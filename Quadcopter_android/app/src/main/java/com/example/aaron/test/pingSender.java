
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

public class pingSender extends AbstractNodeMain {

    public boolean active=false;
    public std_msgs.Empty p;
    public float timeStart=0;
    public boolean flag= false;



    Node node;

    public pingSender(){

    }


    public float getTime(){
        return timeStart;
    }
    @Override
    public GraphName getDefaultNodeName() {
        return GraphName.of("pinger");
    }

    @Override
    public void onStart(final ConnectedNode connectedNode) {

        final Publisher<std_msgs.Empty> publisher =
                connectedNode.newPublisher("/ping", p._TYPE);

        p = publisher.newMessage();

        connectedNode.executeCancellableLoop(new CancellableLoop() {


            @Override
            protected void setup() {

            }
            @Override
            protected void loop() throws InterruptedException {

                if (flag==true){

                    publisher.publish(p);
                    timeStart = System.currentTimeMillis();
                    System.out.println("Ping Start:" + timeStart);

                    flag=false;
                }
            }
        });
    }


}
