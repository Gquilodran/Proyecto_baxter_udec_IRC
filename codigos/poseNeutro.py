import rospy
import baxter_interface

def move_to_neutral():
    print("Iniciando nodo de comunicación con Baxter...")
    rospy.init_node('baxter_to_neutral')

    # Crear instancias para ambos brazos
    left_arm = baxter_interface.Limb('left')
    right_arm = baxter_interface.Limb('right')

    print("Moviendo brazos a posición neutral suavemente...")
    
    # move_to_neutral() calcula una trayectoria suave internamente
    # No es un movimiento brusco, lo cual es ideal para no forzar los joints
    left_arm.move_to_neutral()
    right_arm.move_to_neutral()

    print("¡Listo! Baxter está en posición normal.")

if __name__ == '__main__':
    try:
        move_to_neutral()
    except rospy.ROSInterruptException:
        pass
    
##codigo robado de IA ya que iba en el bus y ya sin cerebro para cerebrar