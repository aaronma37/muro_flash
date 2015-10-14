package com.example.aaron.test;

import org.ros.concurrent.CancellableLoop;
import org.ros.message.MessageDeclaration;
import org.ros.message.Time;
import org.ros.namespace.GraphName;
import org.ros.node.AbstractNodeMain;
import org.ros.node.ConnectedNode;
import org.ros.node.Node;
import org.ros.node.topic.Publisher;

import java.util.ArrayList;
import java.util.List;

import geometry_msgs.Pose;
import geometry_msgs.PoseArray;

public class PathPublisher extends AbstractNodeMain {

    public double x,y;
    public int active=0;
    public geometry_msgs.PoseArray pose;
    public geometry_msgs.Pose intPose;
    public dummyPoseArray dPose;
    public boolean swarm=false;
    public boolean flag= false;



    Node node;

    public PathPublisher()  {
        dPose=new dummyPoseArray();
    }

    @Override
    public GraphName getDefaultNodeName() {
        return GraphName.of("pathPublisher");
    }

    public void setPathArray(dummyPoseArray p){
        dPose=p;
    }

    @Override
    public void onStart(final ConnectedNode connectedNode) {

        final Publisher<geometry_msgs.PoseArray> publisher =
                connectedNode.newPublisher("/path", pose._TYPE);

        pose = publisher.newMessage();
        for (int i=0;i<200;i++){
            intPose = connectedNode.getTopicMessageFactory().newFromType(geometry_msgs.Pose._TYPE);
            pose.getPoses().add(intPose);
        }

        // This CancellableLoop will be canceled automatically when the node shuts
        // down.
        connectedNode.executeCancellableLoop(new CancellableLoop() {


            @Override
            protected void setup() {

            }
            @Override
            protected void loop() throws InterruptedException {


                if (flag==true){
                    if (dPose.header.equals("OPEN")||dPose.header.equals("CLOSED")){
                        for (int i=0;i<200;i++){
                            pose.getPoses().get(i).getPosition().setX(dPose.pose[i].x);
                            pose.getPoses().get(i).getPosition().setY(dPose.pose[i].y);
                            pose.getPoses().get(i).getPosition().setZ(dPose.pose[i].z);

                            pose.getPoses().get(i).getOrientation().setX(dPose.pose[i].ax);
                            pose.getPoses().get(i).getOrientation().setY(dPose.pose[i].ay);
                            pose.getPoses().get(i).getOrientation().setZ(dPose.pose[i].az);
                            pose.getPoses().get(i).getOrientation().setW(dPose.pose[i].aw);
                        }
                    }

                    pose.getHeader().setFrameId(dPose.header);

                    if (swarm==true){
                        pose.getHeader().setFrameId("SWARM");
                    }

                    pose.getHeader().setSeq(0);
                    pose.getHeader().setStamp(Time.fromMillis(System.currentTimeMillis()));
                    if (pose.getPoses().get(0).getPosition().getX()!=0  && pose.getPoses().get(0).getPosition().getY()!=0  ){
                        publisher.publish(pose);
                    }
                    flag=false;
                }
                Thread.sleep(1000);
            }
        });
    }


}
