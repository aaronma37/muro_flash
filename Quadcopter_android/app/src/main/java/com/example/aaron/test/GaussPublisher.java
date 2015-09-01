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

/**
 * Created by sumegamandadi on 8/12/15.
 */
public class GaussPublisher extends AbstractNodeMain {

    private float[] xLocs;
    private float[] yLocs;
    private float[] scales;
    private float[] stdDevs;
    public geometry_msgs.Pose intPose;
    private float OGsize = 0.4f;
    public int active=0;

    public geometry_msgs.PoseArray pose;

    public GaussPublisher(){

    }

    public void getGuassData(float[] x, float[] y, float[] s){
        xLocs = x;
        yLocs = y;
        scales = s;

        //connvert scales to std deviations
        for (int i= 0; i<100; i++){
            stdDevs[i] = scales[i]*OGsize;
        }
    }

    @Override
    public GraphName getDefaultNodeName() {
        return GraphName.of("gaussPublisher");
    }

    @Override
    public void onStart(final ConnectedNode connectedNode) {

        final Publisher<geometry_msgs.PoseArray> publisher =
                connectedNode.newPublisher("/gauss", pose._TYPE);

        pose = publisher.newMessage();

        //pose = publisher.newMessage();

        for (int i=0;i<200;i++){
            intPose = connectedNode.getTopicMessageFactory().newFromType(geometry_msgs.Pose._TYPE);
            pose.getPoses().add(intPose);
        }

        connectedNode.executeCancellableLoop(new CancellableLoop() {


            @Override
            protected void setup() {
                //num= 0;
            }

            @Override
            protected void loop() throws InterruptedException {

                //geometry_msgs.PoseStamped pose = publisher.newMessage();
                pose.getHeader().setFrameId("s");
                pose.getHeader().setStamp(Time.fromMillis(System.currentTimeMillis()));
                for (int i = 0; i<100; i++) {

                    pose.getPoses().get(i).getOrientation().setW(1);
                    pose.getPoses().get(i).getOrientation().setW(0);
                    pose.getPoses().get(i).getOrientation().setW(0);
                    pose.getPoses().get(i).getOrientation().setW(0);


                    pose.getPoses().get(i).getPosition().setX(xLocs[i]);
                    pose.getPoses().get(i).getPosition().setX(yLocs[i]);
                    pose.getPoses().get(i).getPosition().setX(stdDevs[i]);



/*                    if (gaussflag == 1) {
                        publisher.publish(pose);

                    }*/
                }

            }
        });
    }
}






