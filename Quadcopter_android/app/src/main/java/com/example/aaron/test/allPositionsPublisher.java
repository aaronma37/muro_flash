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

public class allPositionsPublisher extends AbstractNodeMain {

    public double x,y;
    public int active=0;
    public geometry_msgs.PoseArray pose;
    public geometry_msgs.Pose intPose;
    public dummyPoseArray dPose;
    public boolean flag= false;



    Node node;

    public allPositionsPublisher(){
        dPose=new dummyPoseArray();
    }

    @Override
    public GraphName getDefaultNodeName() {
        return GraphName.of("allPositionsPublisherNode");
    }

    public void setPositions(turtle[] turtleList){
        for (int i = 0; i <turtleList.length;i++){
            dPose.pose[i].x=turtleList[i].getX();
            dPose.pose[i].y=turtleList[i].getY();
        }
    }

    @Override
    public void onStart(final ConnectedNode connectedNode) {

        final Publisher<geometry_msgs.PoseArray> publisher =
                connectedNode.newPublisher("/toVoronoiDeployment", pose._TYPE);

        pose = publisher.newMessage();
        for (int i=0;i<50;i++){
            intPose = connectedNode.getTopicMessageFactory().newFromType(geometry_msgs.Pose._TYPE);
            pose.getPoses().add(intPose);
        }

        connectedNode.executeCancellableLoop(new CancellableLoop() {


            @Override
            protected void setup() {

            }
            @Override
            protected void loop() throws InterruptedException {

                if (flag==true){
                        for (int i=0;i<50;i++){
                            if (i<dPose.pose.length){
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
                    pose.getHeader().setSeq(0);
                    pose.getHeader().setStamp(Time.fromMillis(System.currentTimeMillis()));
                    publisher.publish(pose);
                    flag=false;
                }
                Thread.sleep(20);
            }
        });
    }


}
