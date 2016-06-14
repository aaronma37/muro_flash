

	// TODO Transform macht hier noch keinen Sinn
	poseMsgToTF(pose_prop.pose.pose, prop_tf);
	prop_tf.getOrigin().setZ(0.0);
	br.sendTransform(tf::StampedTransform(prop_tf, ros::Time::now(), "world", pose_prop.header.frame_id));
	//http://wiki.ros.org/tf/Tutorials/Writing%20a%20tf%20broadcaster%20(C%2B%2B)
	//rosrun tf tf_echo /world /turtle1






    	// only update filter when one of the sensors is active
	if (prop_active || exter_active){

		// update filter at time where all sensor measurements are available
		if (prop_active)  filter_stamp = min(filter_stamp, prop_stamp);
		if (exter_active) filter_stamp = min(filter_stamp, exter_stamp);
		if (update_filter(filter_stamp))
		{
//DELETED:  output most recent estimate and relative covariance			Do not understand this. Why publish?
		}


	}
