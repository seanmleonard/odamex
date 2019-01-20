from numpy import (
    int8, uint8,
    int16, uint16,
    int32, uint32,
    int64, uint64,
)

class fixed_t(int32):
    pass


class angle_t(uint32):
    pass


class V3Fixed(object):
    def __init__(self, x=0, y=0, z=0):
        self.x = fixed_t(x)
        self.y = fixed_t(y)
        self.z = fixed_t(z)

    def __add__(self, other):
        return self.__class__(self.x + other.x, self.y + other.y, self.z + other.z)

    def __sub__(self, other):
        return self.__class__(self.x - other.x, self.y - other.y, self.z - other.z)


class EntityProperty(object):
    def __init__(self, name, cls_type, default, *args, **kwargs):
        self.name = name
        self.cls_type = cls_type
        self.default = default
        self.serialize = True

    def instantiate(self):
        pass


class AActor():
    PROPERTIES = {
        'position': {
            'cls_type': V3Fixed,
            'default': V3Fixed(),
        },
        'health': {
            'cls_type': fixed_t,
            'default': 0,
        },
        'angle': {
            'cls_type': angle_t,
            'default': 0,
        },
    }

    def __init__(self):
        self.properties = {}
        for name, attrs in self.PROPERTIES.items():
            prop = EntityProperty(name=name, **attrs)
            self.properties[name] = prop
            setattr(self, name, prop.default)

    def __setattr__(self, name, value):
        try:
            cls_type = self.properties[name].cls_type
            if not isinstance(value, cls_type):
                value = cls_type(value)
        except (AttributeError, KeyError):
            pass
        super().__setattr__(name, value)

    @classmethod
    def from_dict(cls, actor_dict):
        actor = cls()
        actor.position = V3Fixed(actor_dict['x'], actor_dict['y'], actor_dict['z'])
        actor.angle = actor_dict['angle']
        actor.health = actor_dict['health']
        return actor



def run_actor_thinker(actor_dict):
    raise Exception('testing')
    #actor = AActor.from_dict(actor_dict)
    #actor.health = 200
    #actor.angle += 100*65536;
    #actor_dict['health'] = actor.health
    #actor_dict['angle'] = actor.angle
    actor_dict['health'] = 200
    return 0



if __name__ == '__main__':
    actor_dict = {'angle': 100*65536, 'health': 100, 'x': 50, 'y': -50, 'z': 0}
    run_actor_thinker(actor_dict)
    import pdb; pdb.set_trace()
    pass
