import argparse
import random
import os 

import rospy
import cv2 
import cv_bridge  

import baxter_interface
from baxter_interface import CHECK_VERSION
from sensor_msgs.msg import Image 


class Wobbler(object):

    def __init__(self, image_path):  
        """
        'Wobbles' the head
        """
        self._done = False
        self._head = baxter_interface.Head()
        self._image_path = image_path  
        self._pub = rospy.Publisher('/robot/xdisplay', Image, latch=True, queue_size=1)

        # verify robot is enabled
        print("Getting robot state... ")
        self._rs = baxter_interface.RobotEnable(CHECK_VERSION)
        self._init_state = self._rs.state().enabled
        print("Enabling robot... ")
        self._rs.enable()
        print("Running. Ctrl-c to quit")

    def clean_shutdown(self):
        """
        Exits example cleanly by moving head to neutral position and
        maintaining start state
        """
        print("\nExiting example...")
        if self._done:
            self.set_neutral()
        if not self._init_state and self._rs.state().enabled:
            print("Disabling robot...")
            self._rs.disable()

    def set_neutral(self):
        """
        Sets the head back into a neutral pose
        """
        self._head.set_pan(0.0)

    def _update_display(self):
        try:
            img = cv2.imread(self._image_path)
            msg = cv_bridge.CvBridge().cv2_to_imgmsg(img, encoding="bgr8")
            self._pub.publish(msg)
        except Exception as e:
            rospy.logerr("Error al cargar o publicar la imagen: %s" % str(e))

    def wobble(self):
        self.set_neutral()
        """
        Performs the wobbling
        """
        self._head.command_nod()
        command_rate = rospy.Rate(1)
        control_rate = rospy.Rate(100)
        start = rospy.get_time()
        
        while not rospy.is_shutdown() and (rospy.get_time() - start < 10.0):
            angle = random.uniform(-1.5, 1.5)
            
            self._update_display()
            
            while (not rospy.is_shutdown() and
                   not (abs(self._head.pan() - angle) <=
                       baxter_interface.HEAD_PAN_ANGLE_TOLERANCE)):
                self._head.set_pan(angle, speed=0.3, timeout=0)
                control_rate.sleep()
            command_rate.sleep()

        self._done = True
        rospy.signal_shutdown("Example finished.")


def main():
    """RSDK Head Example: Wobbler with Display

    Nods the head, pans side-to-side towards random angles,
    and updates the face display on every turn.
    """
    arg_fmt = argparse.RawDescriptionHelpFormatter
    parser = argparse.ArgumentParser(formatter_class=arg_fmt,
                                     description=main.__doc__)


    parser.add_argument(
        '-f', '--file', metavar='PATH', required=False,
        default='/home/usuario/imagenes/mi_imagen.png',  
        help='Path to image file to send'
    )
    args = parser.parse_args(rospy.myargv()[1:])

    print("Initializing node... ")
    rospy.init_node("rsdk_head_wobbler")

    if not os.access(args.file, os.R_OK):
        rospy.logerr("Cannot read file at '%s'" % (args.file,))
        return 1

    wobbler = Wobbler(args.file)
    rospy.on_shutdown(wobbler.clean_shutdown)
    print("Wobbling... ")
    wobbler.wobble()
    print("Done.")

if __name__ == '__main__':
    main()