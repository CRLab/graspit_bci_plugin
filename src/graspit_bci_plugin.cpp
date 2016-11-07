
#include "graspit_bci_plugin/graspit_bci_plugin.h"

#include <include/mytools.h>
#include <include/world.h>
#include <include/body.h>
#include <include/graspitCore.h>
#include <include/ivmgr.h>
#include <bciService.h>
#include <bciControlWindow.h>


#include <QtGui>
#include <QFrame>
#include <QWidget>


namespace graspit_bci_plugin
{

GraspitBCIPlugin::GraspitBCIPlugin():
    isInited(false)
{
    ROS_INFO("BCI PLUGIN STARTING");
}

GraspitBCIPlugin::~GraspitBCIPlugin()
{
    ROS_INFO("ROS GraspIt node stopping");
    ros::shutdown();
}


int GraspitBCIPlugin::init(int argc, char **argv)
{

    server_thread = new QThread;
    BCIControlServer *worker = new BCIControlServer();
    worker->moveToThread(server_thread);
    QObject::connect(server_thread, SIGNAL(started()), worker, SLOT(process()));
    server_thread->start();


    qRegisterMetaType<transf>("transf");
    //copy the arguments somewhere else so we can pass them to ROS

    ros::init(argc, argv, "graspit_bci_plugin_node");
    root_nh_ = new ros::NodeHandle("graspit_bci_plugin_node");




    ROS_INFO("about to load world");
    //this should go away, just a hack
    std::string world_name;
    root_nh_->getParam("/world_file", world_name);
    QString worldfile = QString(getenv("GRASPIT")) + QString(world_name.c_str());
    ROS_INFO("%s", worldfile.toStdString().c_str());
    graspitCore->getWorld()->load(worldfile.toStdString().c_str());




    ROS_INFO("Finished initing BCI Plugin");

    return 0;
}

int GraspitBCIPlugin::mainLoop()
{
    if(!isInited)
    {
        isInited = true;
        BCIControlWindow *bciControlWindow= new BCIControlWindow();
        BCIService::getInstance()->init(bciControlWindow);
        bciControlWindow->show();
    }
    ros::spinOnce();
    return 0;
}


}
