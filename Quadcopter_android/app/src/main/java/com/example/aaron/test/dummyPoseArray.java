package com.example.aaron.test;

/**
 * Created by aaron on 8/7/15.
 */
public class dummyPoseArray {
    public dummyPose pose[]= new dummyPose[1000];
    public String header;


    dummyPoseArray(){
        for (int i=0;i<1000;i++){
            pose[i]=new dummyPose();
        }
        header="NOT_INITIALIZED";
    }

    public void Clear(){

        for (int i=0;i<1000;i++){
            pose[i].x=0;
            pose[i].z=0;
            pose[i].y=0;
            pose[i].aw=0;
            pose[i].ax=0;
            pose[i].ay=0;
            pose[i].az=0;
            pose[i].active=false;
            pose[i].direction=0;
        }
        header="NOT_INITIALIZED";
    }
}
