package com.example.aaron.test;

import org.ros.message.MessageListener;
import org.ros.namespace.GraphName;
import org.ros.node.ConnectedNode;
import org.ros.node.Node;
import org.ros.node.NodeMain;
import org.ros.node.topic.Subscriber;
import geometry_msgs.PoseArray;

public class multipleGoalListener<T> implements NodeMain {


    public String messageType;
    public dummyPoseArray dummyArray;
    public boolean flag=false;

    public multipleGoalListener() {
        messageType= geometry_msgs.PoseArray._TYPE;
        dummyArray= new dummyPoseArray();
    }

    @Override
    public GraphName getDefaultNodeName() {
        return GraphName.of("VoronoiGoalFinder");
    }

    @Override
    public void onStart(ConnectedNode connectedNode) {
        Subscriber<geometry_msgs.PoseArray> subscriber = connectedNode.newSubscriber("Centroids", messageType);
        subscriber.addMessageListener(new MessageListener<PoseArray>() {
            @Override
            public void onNewMessage(geometry_msgs.PoseArray poseArray) {
                if (flag==true){
                    dummyArray.Clear();
                    for (int i =0;i<poseArray.getPoses().size();i++){
                        dummyArray.pose[i].x=(float)poseArray.getPoses().get(i).getPosition().getX();
                        dummyArray.pose[i].y=(float)poseArray.getPoses().get(i).getPosition().getY();
                        if (dummyArray.pose[i].y!=0 && dummyArray.pose[i].x!=0){
                            dummyArray.pose[i].active=true;
                        }
                    }
                }

            }
        }, 10);
    }

    @Override
    public void onShutdown(Node node) {
    }

    @Override
    public void onShutdownComplete(Node node) {
    }

    @Override
    public void onError(Node node, Throwable throwable) {
    }
}