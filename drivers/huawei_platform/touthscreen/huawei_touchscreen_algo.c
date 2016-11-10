#include "huawei_touchscreen_algo.h"
#include <linux/ktime.h>

#define EDGE_Y_MID		1380
#define EDGE_X_MAX		1079
//#define EDGE_STOP_L		10
//#define EDGE_STOP_R		(EDGE_X_MAX - EDGE_STOP_L)
//#define EDGE_X_LEFT		(EDGE_STOP_L + 20)
//#define DEGE_X_RIGHT		(EDGE_STOP_R - 20)

//#define START_UX_LEFT		(EDGE_STOP_L + 30)
//#define START_UX_RIGHT	(EDGE_STOP_R - 30)
//#define START_DX_LEFT		(EDGE_STOP_L + 40)
//#define START_DX_RIGHT	(EDGE_STOP_R - 40)
#define STOP_LIMITED	100

#define Y_MOVE_S		20
#define Y_MOVE_M		40

//#define TEMP_X_LEFT		(EDGE_STOP_L + 15)
//#define TEMP_X_RIGHT	(EDGE_STOP_R - 15)

#define CENTER_X		500
#define OFFSET_DIS		100
#define RES_ADDR		0
#define RES_ERT			1
#define RES_ERF			2
#define RES_CRF			3
#define RES_ERT_THR		2

int edge_stop_lt = 0;
int edge_stop_rt = 0;
int edge_ax_left = 0;
int edge_ax_right = 0;
int start_upx_left = 0;
int start_upx_right = 0;
int start_dnx_left = 0;
int start_dnx_right = 0;

extern struct ts_data g_ts_data;
static struct timespec curr_time[FILTER_GLOVE_NUMBER] = { {0, 0} };

static struct timespec pre_finger_time[FILTER_GLOVE_NUMBER] = { {0, 0} };

static int touch_pos_x[FILTER_GLOVE_NUMBER] = { -1, -1, -1, -1 };
static int touch_pos_y[FILTER_GLOVE_NUMBER] = { -1, -1, -1, -1 };

static enum TP_state_machine touch_state = INIT_STATE;

static u16 must_report_flag = 0;
static u16 pre_must_flag = 0;
static u16 stop_report_flag = 0;
static u16 temp_report_flag = 0;
static int finger_stop_cnt[TS_MAX_FINGER] = {0};
static int finger_stop_y[TS_MAX_FINGER] = {0};

static int left_res_point[4] = {0};		//0 point addr /1 error times /2 error flag /3 correct flag
static int right_res_point[4] = {0};
static int ts_algo_filter_anti_false_touch_edge(int x, struct anti_false_touch_param *param){
	int drv_limit_x_left = 0, drv_limit_x_right = 0;

	if (!param || !param->feature_all){
		return NOT_AFT_EDGE;
	}

	drv_limit_x_left = param->drv_stop_width;
	drv_limit_x_right = param->lcd_width - drv_limit_x_left;
	if (x < drv_limit_x_left ||x > drv_limit_x_right) {
		TS_LOG_DEBUG("%s edge, x:%d\n", __func__, x);
		return AFT_EDGE;
	}
	return NOT_AFT_EDGE;
}

static int filter_illegal_glove(u8 n_finger, struct ts_fingers *in_info)
{
	u8 report_flag = 0;
	long interval_time;
	u8 new_mode;
	int x = in_info->fingers[n_finger].x;
	int y = in_info->fingers[n_finger].y;

	new_mode = in_info->fingers[n_finger].status;
	if (new_mode == TP_FINGER || g_ts_data.feature_info.holster_info.holster_switch) {	/*the new interrupt is a finger signal */
		touch_state = FINGER_STATE;
		report_flag = 1;
	} else if ((new_mode == TP_GLOVE) || (new_mode == TP_STYLUS)) {	/*the new interrupt is a glove signal. */
		switch (touch_state) {
		case INIT_STATE:
			report_flag = 1;
			touch_state = GLOVE_STATE;
			break;

		case FINGER_STATE:
			ktime_get_ts(&curr_time[n_finger]);
			interval_time =
			    (curr_time[n_finger].tv_sec -
			     pre_finger_time[n_finger].tv_sec) * 1000 +
			    (curr_time[n_finger].tv_nsec -
			     pre_finger_time[n_finger].tv_nsec) / 1000000;
			if (interval_time > 0
			    && interval_time <= FINGER_REL_TIME) {
				ktime_get_ts(&pre_finger_time[n_finger]);
			} else {
				touch_state = ZERO_STATE;
			}
			break;

		case ZERO_STATE:
			if ((touch_pos_x[n_finger] == -1)
			    && (touch_pos_y[n_finger] == -1)) {
				touch_pos_x[n_finger] = x;
				touch_pos_y[n_finger] = y;
			} else {
				if (((touch_pos_x[n_finger] -
				      x) * (touch_pos_x[n_finger] - x)
				     + (touch_pos_y[n_finger] -
					y) * (touch_pos_y[n_finger] - y))
				    >= (PEN_MOV_LENGTH * PEN_MOV_LENGTH)) {
					touch_state = GLOVE_STATE;
				}
			}
			break;

		case GLOVE_STATE:
			report_flag = 1;
			break;

		default:
			TS_LOG_ERR("error: touch_state = %d\n", touch_state);
			break;
		}
	} else {
		TS_LOG_DEBUG("[%s]:error:cur_mode=%d\n", __func__,new_mode);
		report_flag = 1;
	}

	return report_flag;
}

int ts_algo_t2(struct ts_device_data *dev_data, struct ts_fingers *in_info,
	       struct ts_fingers *out_info)
{
	int index;
	int id = 0;
	struct anti_false_touch_param *local_param = NULL;

	if (NULL == dev_data){
		TS_LOG_DEBUG("%s anti false touch get chip data NULL\n", __func__);
		local_param = NULL;
	}else{
		local_param = &(dev_data->anti_false_touch_param_data);
	}


	for (index = 0, id = 0; index < TS_MAX_FINGER; index++, id++) {
		if (in_info->cur_finger_number == 0) {
			out_info->fingers[0].status = TS_FINGER_RELEASE;
			if (id >= 1)
				out_info->fingers[id].status = 0;
		} else {
			if (((in_info->fingers[index].x != 0) ||(in_info->fingers[index].y != 0))
				&& (NOT_AFT_EDGE == ts_algo_filter_anti_false_touch_edge(in_info->fingers[index].x, local_param))) {
				out_info->fingers[id].x = in_info->fingers[index].x;
				out_info->fingers[id].y = in_info->fingers[index].y;
				out_info->fingers[id].pressure = in_info->fingers[index].pressure;
				if (local_param && local_param->feature_all){
					out_info->fingers[id].major = in_info->fingers[index].major;
					out_info->fingers[id].minor = in_info->fingers[index].minor;
				}
				out_info->fingers[id].status = TS_FINGER_PRESS;
			} else
				out_info->fingers[id].status = 0;
		}
	}

	out_info->gesture_wakeup_value = in_info->gesture_wakeup_value;
	out_info->special_button_key = in_info->special_button_key;
	out_info->special_button_flag = in_info->special_button_flag;

	return NO_ERR;
}

int ts_algo_t1(struct ts_device_data *dev_data, struct ts_fingers *in_info,
	       struct ts_fingers *out_info)
{
	int index;
	int id;
	int finger_cnt  = 0;
	struct anti_false_touch_param *local_param = NULL;

	if (NULL == dev_data){
		TS_LOG_ERR("%s anti false touch get chip data NULL\n", __func__);
		local_param = NULL;
	}else{
		local_param = &(dev_data->anti_false_touch_param_data);
	}

	for (index = 0, id = 0; index < TS_MAX_FINGER; index++, id++) {
		if (in_info->cur_finger_number == 0) {
			if (index < FILTER_GLOVE_NUMBER) {
				touch_pos_x[index] = -1;
				touch_pos_y[index] = -1;
				if (touch_state == FINGER_STATE) {	/*this is a finger release */
					ktime_get_ts(&pre_finger_time[index]);
				}
			}
			out_info->fingers[0].status = TS_FINGER_RELEASE;
			if (id >= 1)
				out_info->fingers[id].status = 0;
		} else {
			if (((in_info->fingers[index].x != 0) ||(in_info->fingers[index].y != 0))
				&& (NOT_AFT_EDGE == ts_algo_filter_anti_false_touch_edge(in_info->fingers[index].x, local_param))) {
				if (index < FILTER_GLOVE_NUMBER) {
					if (filter_illegal_glove(index, in_info)
					    == 0) {
						out_info->fingers[id].status =
						    0;
					} else {
						finger_cnt++;
						out_info->fingers[id].x = in_info->fingers[index].x;
						out_info->fingers[id].y = in_info->fingers[index].y;
						out_info->fingers[id].pressure = in_info->fingers[index].pressure;
						if (local_param && local_param->feature_all){
							out_info->fingers[id].major = in_info->fingers[index].major;
							out_info->fingers[id].minor = in_info->fingers[index].minor;
						}
						out_info->fingers[id].status = TS_FINGER_PRESS;
					}
				} else {
					finger_cnt++;
					out_info->fingers[id].x = in_info->fingers[index].x;
					out_info->fingers[id].y = in_info->fingers[index].y;
					out_info->fingers[id].pressure = in_info->fingers[index].pressure;
					if (local_param && local_param->feature_all){
						out_info->fingers[id].major = in_info->fingers[index].major;
						out_info->fingers[id].minor = in_info->fingers[index].minor;
					}
					out_info->fingers[id].status = TS_FINGER_PRESS;
				}
			} else
				out_info->fingers[id].status = 0;
		}
	}

	out_info->cur_finger_number = finger_cnt;
	out_info->gesture_wakeup_value = in_info->gesture_wakeup_value;
	out_info->special_button_key = in_info->special_button_key;
	out_info->special_button_flag = in_info->special_button_flag;

	return NO_ERR;
}

int stop_to_start(int x, int y, int start_y, int cnt, int *point)
{
	int temp_value;
	temp_value = y - start_y;

	if (!((y > point[RES_ADDR] - OFFSET_DIS) && (y < point[RES_ADDR] + OFFSET_DIS)))
//	if (y < EDGE_Y_MID)
	{
		if (temp_value > Y_MOVE_S)
		{
			return 1;
		}
		if (temp_value < 0)
		{
			temp_value = 0 - temp_value;
			if (temp_value > Y_MOVE_M)
			{
				return 2;
			}
		}
		if ((x > start_upx_left) && (x < start_upx_right))
		{
			return 3;
		}
	}
	else
	{
		if ((x > (start_dnx_left + cnt)) && (x < (start_dnx_right - cnt)))
		{
			return 4;
		}
	}
	return 0;
}

int start_stop_area(int x, int y, int *point)
{
	if (x < edge_stop_lt || x > edge_stop_rt)
	{
		return 1;
	}
//	else if (y > EDGE_Y_MID)
//	{
//		if ((y > point[RES_ADDR] - OFFSET_DIS) && (y < point[RES_ADDR] + OFFSET_DIS))
//		{
//			return 1;
//		}
//	}
	return 0;
}

int update_restrain_area(int y, int *point)
{
	if (y > EDGE_Y_MID)													//restrain area at the bottom screen
	{
		if (0 == point[RES_ADDR])										//restrain area not exist, set it up
		{
			point[RES_CRF] = 1;											//correct flag
			point[RES_ADDR] = y;
		}
		else if ((y > point[RES_ADDR] - OFFSET_DIS) && (y < point[RES_ADDR] + OFFSET_DIS))
		{																//adust the area
			point[RES_CRF] = 1;											//correct flag
			point[RES_ADDR] = (point[RES_ADDR]*3 + y) >> 2;

			TS_LOG_DEBUG("restrain point updated: %d\n", point[RES_ADDR]);
		}
		else															//not in the current restrain area
		{
			point[RES_ERF] = 1;											//error flag
		}
	}
}

int ts_algo_t3(struct ts_device_data *dev_data, struct ts_fingers *in_info, struct ts_fingers *out_info)
{
	int index;
	int temp_x, temp_y, temp_val;
	int *temp_point;

	if (0 == g_ts_data.edge_wideth)
	{
		return NO_ERR;
	}
	if (g_ts_data.edge_wideth != edge_stop_lt)
	{
		edge_stop_lt = g_ts_data.edge_wideth;
		edge_stop_rt = EDGE_X_MAX - edge_stop_lt;
		edge_ax_left = edge_stop_lt + 20;
		edge_ax_right = edge_stop_rt - 20;

		start_upx_left = edge_stop_lt + 30;
		start_upx_right = edge_stop_rt - 30;

		start_dnx_left = edge_stop_lt + 40;
		start_dnx_right = edge_stop_rt - 40;
	}

	if (in_info->cur_finger_number == 0)
	{
		TS_LOG_DEBUG("no finger, only a release issue\n");

		if ((left_res_point[RES_CRF] == 0) && (left_res_point[RES_ERF] != 0))
		{
			if (left_res_point[RES_ERT] < RES_ERT_THR)
			{
				left_res_point[RES_ERT]++;
				if (left_res_point[RES_ERT] >= RES_ERT_THR)
				{
					left_res_point[RES_ADDR] = 0;
					left_res_point[RES_ERT] = 0;
				}
			}
		}
		if ((right_res_point[RES_CRF] == 0) && (right_res_point[RES_ERF] != 0))
		{
			if (right_res_point[RES_ERT] < RES_ERT_THR)
			{
				right_res_point[RES_ERT]++;
				if (right_res_point[RES_ERT] >= RES_ERT_THR)
				{
					right_res_point[RES_ADDR] = 0;
					right_res_point[RES_ERT] = 0;
				}
			}
		}
		left_res_point[RES_CRF] = 0;
		left_res_point[RES_ERF] = 0;
		right_res_point[RES_CRF] = 0;
		right_res_point[RES_ERF] = 0;

		must_report_flag = 0;
		stop_report_flag = 0;
		temp_report_flag = 0;
		for (index = 0; index < TS_MAX_FINGER; index++)
		{
			finger_stop_cnt[index] = 0;
			finger_stop_y[index] = 0;
		}
	}
	else
	{
		for (index = 0; index < TS_MAX_FINGER; index++)
		{
			if (in_info->fingers[index].status != 0)
			{
				temp_x = in_info->fingers[index].x;
				temp_y = in_info->fingers[index].y;
				if (must_report_flag & (1 << index))
				{
					TS_LOG_DEBUG("finger index: %d, is reporting \n", index);

					out_info->fingers[index].x = temp_x;
					out_info->fingers[index].y = temp_y;
					out_info->fingers[index].pressure = in_info->fingers[index].pressure;
					out_info->fingers[index].status = TS_FINGER_PRESS;
				}
				else
				{
					temp_point = left_res_point;
					if (temp_x > CENTER_X)
						temp_point = right_res_point;

					if (stop_report_flag & (1 << index))
					{
						temp_val = stop_to_start(temp_x, temp_y, finger_stop_y[index], finger_stop_cnt[index], temp_point);
						TS_LOG_DEBUG("stop_to_start ret_value: %d, \n", temp_val);
						if (temp_val)
						{
							TS_LOG_DEBUG("stopped finger index: %d, coordinate is legal1, can report again \n", index);

							must_report_flag |= (1 << index);
							stop_report_flag &= ~(1 << index);

							out_info->fingers[index].x = temp_x;
							out_info->fingers[index].y = temp_y;
							out_info->fingers[index].pressure = in_info->fingers[index].pressure;
							out_info->fingers[index].status = TS_FINGER_PRESS;
						}
						else
						{
							out_info->fingers[index].status = 0;
							if (finger_stop_cnt[index] < STOP_LIMITED)
								finger_stop_cnt[index]++;
							TS_LOG_DEBUG("finger index: %d, keep stopped, stop_cnt: %d \n", index, finger_stop_cnt[index]);

							update_restrain_area(temp_y, temp_point);
						}
					}
					else
					{
						if (temp_x < edge_ax_left || temp_x > edge_ax_right)					//this area maybe need to restrain
						{
							TS_LOG_DEBUG("finger index: %d, stop judge\n", index);

							out_info->fingers[index].status = 0;								//don't report first (important)

							if (!(temp_report_flag & (1 << index)))								//has been report first
							{
								if (start_stop_area(temp_x, temp_y, temp_point))
								{																//current point in direct restrain area
									TS_LOG_DEBUG("finger need stopped \n");

									stop_report_flag |= 1 << index;
									finger_stop_cnt[index] = 1;
									finger_stop_y[index] = temp_y;

									update_restrain_area(temp_y, temp_point);
								}
								else
								{
									temp_report_flag |= 1 << index;
								}
							}
						}
						else
						{
							TS_LOG_DEBUG("finger index: %d, start OK, directly report \n", index);

							if ((!(temp_report_flag & (1 << index))) ||								//not in edge area forever
								((temp_x > start_upx_left ) && (temp_x < start_upx_right)))			//not in edge area now
							{
									must_report_flag |= 1 << index;
									temp_report_flag &= ~(1 << index);
							}

							out_info->fingers[index].x = in_info->fingers[index].x;
							out_info->fingers[index].y = in_info->fingers[index].y;
							out_info->fingers[index].pressure = in_info->fingers[index].pressure;
							out_info->fingers[index].status = TS_FINGER_PRESS;
						}
					}
				}
			}
			else {
				out_info->fingers[index].status = 0;

				must_report_flag &= ~(1 << index);
				stop_report_flag &= ~(1 << index);
				temp_report_flag &= ~(1 << index);
				finger_stop_cnt[index] = 0;
				finger_stop_y[index] = 0;
			}
		}

		TS_LOG_DEBUG("1_must_report_flag=%d, stop=%d, temp=%d, L_RES_ADDR=%d, L_RES_ERT=%d, R_RES_ADDR=%d, R_RES_ERT=%d\n",
			must_report_flag, stop_report_flag, temp_report_flag, left_res_point[RES_ADDR], left_res_point[RES_ERT],
			right_res_point[RES_ADDR], right_res_point[RES_ERT]);

		if (temp_report_flag)
		{
			if (must_report_flag)
			{
				for (index = 0; index < TS_MAX_FINGER; index++)
				{
					//if ((temp_report_flag & (1 << index)) && (!(must_report_flag & (1 << index))))
					if (temp_report_flag & (1 << index))
					{
						if (!(stop_report_flag & (1 << index)))
						{
							finger_stop_cnt[index] = 1;
							finger_stop_y[index] = in_info->fingers[index].y;

							out_info->fingers[index].status = 0;
						}
					}
				}
				stop_report_flag |= temp_report_flag;
				temp_report_flag = 0;

				if (0 == pre_must_flag)
				{
					out_info->fingers[0].status = TS_FINGER_RELEASE;
				}
			}
			else
			{
				for (index = 0; index < TS_MAX_FINGER; index++)
				{
					//if ((temp_report_flag & (1 << index)) && (!(stop_report_flag & (1 << index))))
					if (temp_report_flag & (1 << index))										//temp_report_flag bit won't the same with stop_report_flag bit
					{
						out_info->fingers[index].x = in_info->fingers[index].x;
						out_info->fingers[index].y = in_info->fingers[index].y;
						out_info->fingers[index].pressure = in_info->fingers[index].pressure;
						out_info->fingers[index].status = TS_FINGER_PRESS;
					}
				}
			}
		}

		TS_LOG_DEBUG("2_must_report_flag=%d, stop=%d, temp=%d, L_RES_ADDR=%d, L_RES_ERT=%d, R_RES_ADDR=%d, R_RES_ERT=%d\n",
			must_report_flag, stop_report_flag, temp_report_flag, left_res_point[RES_ADDR], left_res_point[RES_ERT],
			right_res_point[RES_ADDR], right_res_point[RES_ERT]);

		if ((must_report_flag | temp_report_flag)  == 0)
			out_info->fingers[0].status = TS_FINGER_RELEASE;
	}
	pre_must_flag = must_report_flag;
	return NO_ERR;
}

struct ts_algo_func ts_algo_f1 = {
	.algo_name = "ts_algo_f1",
	.chip_algo_func = ts_algo_t1,
};

struct ts_algo_func ts_algo_f2 = {
	.algo_name = "ts_algo_f2",
	.chip_algo_func = ts_algo_t2,
};

struct ts_algo_func ts_algo_f3 =
{
	.algo_name = "ts_algo_f3",
	.chip_algo_func = ts_algo_t3,
};

int ts_register_algo_func(struct ts_device_data *chip_data)
{
	int retval = 0;

	retval = register_algo_func(chip_data, &ts_algo_f1);	/*put algo_f1 into list contained in chip_data, named algo_t1 */
	if (retval < 0) {
		TS_LOG_ERR("alog 1 failed, retval = %d\n", retval);
		return retval;
	}

	retval = register_algo_func(chip_data, &ts_algo_f2);	/*put algo_f2 into list contained in chip_data, named algo_t2 */
	if (retval < 0) {
		TS_LOG_ERR("alog 2 failed, retval = %d\n", retval);
		return retval;
	}

	retval = register_algo_func(chip_data, &ts_algo_f3);	//put algo_f3 into list contained in chip_data, named algo_t3
	if (retval < 0) {
		TS_LOG_ERR("alog 3 failed, retval = %d\n", retval);
		return retval;
	}

	return retval;
}
