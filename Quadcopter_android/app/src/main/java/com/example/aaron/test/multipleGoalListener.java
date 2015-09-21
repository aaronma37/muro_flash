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
        messageType= tf2_msgs.TFMessage._TYPE;
        dummyArray= new dummyPoseArray();
    }

    @Override
    public GraphName getDefaultNodeName() {
        return GraphName.of("VoronoiGoalFinder");
    }

    @Override
    public void onStart(ConnectedNode connectedNode) {
        Subscriber<tf2_msgs.TFMessage> subscriber = connectedNode.newSubscriber("Centroids", messageType);
        subscriber.addMessageListener(new MessageListener<tf2_msgs.TFMessage>() {
            @Override
            public void onNewMessage(tf2_msgs.TFMessage poseArray) {
                if (flag==true){
                    dummyArray.Clear();
                    for (int i =0;i<poseArray.getTransforms().size();i++){
                        dummyArray.pose[i].x=(float)poseArray.getTransforms().get(i).getTransform().getTranslation().getX();
                        dummyArray.pose[i].y=(float)poseArray.getTransforms().get(i).getTransform().getTranslation().getY();
                        if (dummyArray.pose[i].y!=0 && dummyArray.pose[i].x!=0){
                            dummyArray.pose[i].active=true;
                        }
                    }
                }

            }
        }, 100);
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