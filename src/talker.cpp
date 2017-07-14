#include "ros/ros.h"
#include "std_msgs/String.h"
#include "std_msgs/Int32.h"
#include "performance_test/SuperAwesome.h"

#include <dynamic_reconfigure/server.h>
#include <performance_test/performance_test_paramsConfig.h>

#include <sstream>

static ros::Rate* loop_rate;

void pubFreqCallback(const std_msgs::Int32::ConstPtr& msg)
{
  ROS_INFO("Got new freq: [%d]", msg->data);
  delete loop_rate;
  loop_rate = new ros::Rate(msg->data);
}


void callback(performance_test::performance_test_paramsConfig &config, uint32_t level) {
  ROS_INFO("Reconfigure Request: %d", config.rate);
  delete loop_rate;
  loop_rate = new ros::Rate(config.rate);
}

/**
 * This tutorial demonstrates simple sending of messages over the ROS system.
 */
int main(int argc, char **argv)
{
  /**
   * The ros::init() function needs to see argc and argv so that it can perform
   * any ROS arguments and name remapping that were provided at the command line.
   * For programmatic remappings you can use a different version of init() which takes
   * remappings directly, but for most command-line programs, passing argc and argv is
   * the easiest way to do it.  The third argument to init() is the name of the node.
   *
   * You must call one of the versions of ros::init() before using any other
   * part of the ROS system.
   */
  ros::init(argc, argv, "talker");

  dynamic_reconfigure::Server<performance_test::performance_test_paramsConfig> server;
  dynamic_reconfigure::Server<performance_test::performance_test_paramsConfig>::CallbackType f;

  f = boost::bind(&callback, _1, _2);
  server.setCallback(f);

  /**
   * NodeHandle is the main access point to communications with the ROS system.
   * The first NodeHandle constructed will fully initialize this node, and the last
   * NodeHandle destructed will close down the node.
   */
  ros::NodeHandle n;

  /**
   * The advertise() function is how you tell ROS that you want to
   * publish on a given topic name. This invokes a call to the ROS
   * master node, which keeps a registry of who is publishing and who
   * is subscribing. After this advertise() call is made, the master
   * node will notify anyone who is trying to subscribe to this topic name,
   * and they will in turn negotiate a peer-to-peer connection with this
   * node.  advertise() returns a Publisher object which allows you to
   * publish messages on that topic through a call to publish().  Once
   * all copies of the returned Publisher object are destroyed, the topic
   * will be automatically unadvertised.
   *
   * The second parameter to advertise() is the size of the message queue
   * used for publishing messages.  If messages are published more quickly
   * than we can send them, the number here specifies how many messages to
   * buffer up before throwing some away.
   */
  ros::Publisher chatter_pub = n.advertise<performance_test::SuperAwesome>("chatter", 1000);

  ros::Subscriber sub = n.subscribe("PubFreq",1000,pubFreqCallback);

  int rate;

  ros::NodeHandle private_node_handle_("~");
  private_node_handle_.param("rate", rate, int(40));

  loop_rate = new ros::Rate(rate);

  /**
   * A count of how many messages we have sent. This is used to create
   * a unique string for each message.
   */
  int count = 0;
  while (ros::ok())
  {
    /**
     * This is a message object. You stuff it with data, and then publish it.
     */
    performance_test::SuperAwesome msg;

    std::stringstream ss;
    ss << "hello world " << count;
    msg.str = ss.str();

    ROS_INFO("%s", msg.str.c_str());

    /**
     * The publish() function is how you send messages. The parameter
     * is the message object. The type of this object must agree with the type
     * given as a template parameter to the advertise<>() call, as was done
     * in the constructor above.
     */
    chatter_pub.publish(msg);

    ros::spinOnce();
    loop_rate->sleep();
    ++count;
  }


  return 0;
}
