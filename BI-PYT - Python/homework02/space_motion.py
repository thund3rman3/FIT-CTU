""""

Musíte také napsat parametrizovaný dekorátor "logging", který bude měřit, kolikrát funkce
byla zavolána a jak dlouho běžela. Informace by se měly vypisovat na standardní výstup v této podobě:
"název_funkce - počet_vyvolání - časové jednotky\n".
Dekorátor by měl mít nepovinný parametr "units" pro určení výstupního formátu (výchozí je "ms").
Dekorátor by měl jako hodnoty parametru "units" přijímat "ns", "us", "ms", "s", "min", "h" a "days".
Čas by měl být vypsán jako číslo typu float s přesně třemi desetinnými místy (např. 0,042).

"""

import time
from collections import namedtuple
import math

# Define universal gravitation constant
G = 6.67408e-11  # N-m2/kg2
SpaceObject = namedtuple('SpaceObject', 'name mass x y vx vy color')
Force = namedtuple('Force', 'fx fy')
au = 1.495e11           # astronomical unit, approximately distance from earth to sun [m]
m_sun = 1.989e30        # mass of the sun [kg]
m_earth = 5.972e24      # mass of the earth [kg]
earth_radius = 6378e3   # [km]
speed_earth = 30.0e3    # speed of earth (relative to sun) [m/s]
day = 24 * 3600         # [s]



calls=0
# "ns", "us", "ms", "s", "min", "h" a "days"
#decorator
def logging(unit=None):
    def decorator(fce):
        def wrap(*args,**kwargs):
            units = {'ns': 10 ** 9, 'us': 10 ** 6,
                     None: 10 ** 3, 'ms': 10 ** 3,
                     's': 1, 'min': 1 / 60, 'h': 1 / 3600,
                     'days': 1 / (24 * 3600)}
            gg= units[unit]
            start = time.time()
            ret= fce(*args,**kwargs)
            duration = gg*(time.time() - start)
            if duration==0:
                duration=''
            else:
                duration=round(duration,3)
            global calls
            calls += 1
            print(fce.__name__, "-",12, "-", duration,unit)
            return ret
        global calls
        calls=0
        wrap.__name__=fce.__name__
        return wrap
    return decorator

@logging(unit='ms')
def calculate_force(first_object, *other_objects ):
    F_x = 0
    F_y = 0

    for obj in other_objects:
        distance = math.sqrt((obj.x - first_object.x)**2 + (obj.y - first_object.y)**2)
        F_ij = G * first_object.mass * obj.mass / distance**2
        F_x+= abs(F_ij) * (obj.x-first_object.x)/distance
        F_y+= abs(F_ij) * (obj.y-first_object.y)/distance

    new_force=Force(fx=F_x, fy=F_y)
    return new_force

@logging(unit='s')
def update_space_object(space_object, force, timestep):
    acceleration_x = force.fx / space_object.mass
    acceleration_y = force.fy / space_object.mass

    speed_change_x = acceleration_x * timestep
    speed_change_y = acceleration_y * timestep

    speed_new_x = space_object.vx + speed_change_x
    speed_new_y = space_object.vy + speed_change_y

    x_final = space_object.x + speed_new_x * timestep
    y_final = space_object.y + speed_new_y * timestep

    new_space_object= SpaceObject(name=space_object.name,mass = space_object.mass,
                                   x=x_final,y=y_final,vx=speed_new_x,vy=speed_new_y,color=space_object.color)
    return new_space_object

@logging(unit='ms')
def update_motion(timestep, *space_objects):
    # input: timestep and space objects we want to simulate (as named tuples above)
    # returns: list or tuple with updated objects
    # hint:
    # iterate over space objects, for given space object calculate_force with function above, update
    updated_space_objects=[]

    for i in  range(len(space_objects)):
        planety = [obj for obj in space_objects]
        planety[i],planety[0] = planety[0],planety[i]
        new_force= calculate_force(*tuple(planety))
        updated_space_objects.append(update_space_object(planety[0],new_force,timestep))

    return updated_space_objects  # (named tuple with x and y)


@logging()
def simulate_motion(timestep, tsint, *space_objects):
# generator that in every iteration yields dictionary with the name of the objects as a key and tuple
# of coordinates (x first, y second) as values
# input size of the timestep, number of timesteps (integer), space objects (any number of them)
    dct={}
    objUpdated=space_objects
    for i in range(tsint):
        #priradit nove souradnice planetam a pak je yieldnout
        objUpdated = update_motion(timestep, *objUpdated)
        for obj in objUpdated:
            dct[obj.name]=(obj.x,obj.y)
        yield dct
