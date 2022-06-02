# python3 -m pip install pygame
import pygame
import time


class Controller(object):
    STICK_DEADBAND = .05
    def __init__(self, axis_map):
        self.joystick = None
        self.axis_map = axis_map
        
    def update(self):
        pygame.event.pump()

    def getThrottle(self):
        return self._getAxis(0)

    def getRoll(self):
        return self._getAxis(1)

    def getPitch(self):
        return self._getAxis(2)

    def getYaw(self):
        return self._getAxis(3)

    def _getAxis(self, k):
        j = self.axis_map[k]
        val = self.joystick.get_axis(abs(j))
        if abs(val) < Controller.STICK_DEADBAND:
            val = 0
        return (-1 if j<0 else +1) * val

    def process_input(self):
        yaw = self.getYaw() * 32767
        roll = self.getRoll() * 32767
        pitch = self.getPitch() * 32767
        lift = self.getThrottle() * 32767
        aux = self._getAuxValue() 
        cmd = f'{roll:.0f}', f'{-pitch:.0f}', f'{yaw:.0f}', f'{-lift:.0f}', str(aux)
        print(cmd)
        return (cmd)


class _GameController(Controller):
    def __init__(self, axis_map, button_id):
        Controller.__init__(self, axis_map)
        self.button_id = button_id
        self.button_is_down = False
        self.switch_value = -1

    def _getAuxValue(self):
        return self.joystick.get_button(self.button_id)
        
    def getAux(self):
        if self._getAuxValue():
            if not self.button_is_down:
                self.switch_value = -self.switch_value
            self.button_is_down = True
        else:
            self.button_is_down = False
        return self.switch_value

class _SpringyThrottleController(_GameController):
    def __init__(self, axis_map, button_id):
        _GameController.__init__(self, axis_map, button_id)
        self.throttleval = -1
        self.prevtime = 0

    def getThrottle(self):
        currtime = time.time()
        # Scale throttle increment by time difference from last update
        self.throttleval += self._getAxis(0) * (currtime-self.prevtime)
        # Constrain throttle to [-1,+1]
        self.throttleval = min(max(self.throttleval, -1), +1)
        self.prevtime = currtime
        return self.throttleval

class _RcTransmitter(Controller):
    def __init__(self, axis_map, aux_id):
        Controller.__init__(self, axis_map)
        self.aux_id = aux_id

    def getAux(self):
        return +1 if self.joystick.get_axis(self.aux_id) > 0 else -1
        
class _Xbox360(_SpringyThrottleController):
    def __init__(self, axes, aux):
        _SpringyThrottleController.__init__(self, axes, None)
        self.aux = aux

    def _getAuxValue(self):
        return self.joystick.get_axis(self.aux) < -.5

class _Playstation(_SpringyThrottleController):
    def __init__(self, axes):
        _SpringyThrottleController.__init__(self, axes, 7)


controllers = {
    'Controller (Rock Candy Gamepad for Xbox 360)'       : _Xbox360((-1,4,-3,0), 2), 
    'Rock Candy Gamepad for Xbox 360'                    : _Xbox360((-1,3,-4,0), 5), 
    '2In1 USB Joystick'                                  : _Playstation((-1,2,-3,0)),
    'Wireless Controller'                                : _Playstation((-1,2,-3,0)),
    'MY-POWER CO.,LTD. 2In1 USB Joystick'                : _Playstation((-1,2,-3,0)),
    'Sony Interactive Entertainment Wireless Controller' : _Playstation((-1,3,-4,0)),
    'Logitech Extreme 3D'                                : _GameController((3,0,1,2), 0),
    'Logitech Logitech Extreme 3D'                       : _GameController((-3,0,-1,2), 0),
    'FrSky Taranis Joystick'                             : _RcTransmitter((0,1,2,5), 3),
    'FrSky FrSky Taranis Joystick'                       : _RcTransmitter((0,1,2,3), 5),
    'SPEKTRUM RECEIVER'                                  : _RcTransmitter((1,2,5,0), 4),
    'Horizon Hobby SPEKTRUM RECEIVER'                    : _RcTransmitter((1,2,3,0), 4),
    'Microsoft X-Box 360 pad'                            : _Xbox360((-1,4,-3,-0), 2)
    }




pygame.display.init()
pygame.joystick.init()
joystick = pygame.joystick.Joystick(0)
joystick.init()
controller_name = joystick.get_name()
print(controller_name)
if not controller_name in controllers.keys():
    print('Unrecognized controller: %s' % controller_name)
    exit(1)
js = controllers[controller_name]
js.joystick = joystick
