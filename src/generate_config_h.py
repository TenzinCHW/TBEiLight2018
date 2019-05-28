import math

NUM_LAMPS = 80

polar_coords = {('D1',(0,0)) :
        {'71':(173, 0), '79':(323, 10), '44':(504, 0), '66':(597, 0), '41':(763, 0), '46':(692, 10), '78':(589, 10), '54':(403, 20), '75':(238, 40), '48':(123, 45), '58':(184, 70), '67':(334, 45), '68':(489, 30), '63':(636, 30), '76':(761, 20), '52':(832, 40), '64':(720, 40), '72':(557, 40), '70':(477, 45), '42':(371, 55), '45':(264, 60), '39':(223, 80), '31':(110, 120), '61':(196, 100), '34':(258, 115), '43':(337, 125), '50':(309, 90), '59':(365, 80), '25':(349, 60), '62':(482, 45), '20':(531, 40), '47':(578, 30), '55':(701, 25), '65':(804, 20), '49':(496, 60), '26':(478, 80), '51':(447, 90), '77':(504, 100), '33':(406, 95), '57':(508, 120), '27':(372, 180), '30':(496, 180), '38':(613, 180), '23':(739, 180), '32':(830, 180), '36':(915, 190), '74':(989, 180), '56':(1092, 185), '28':(1161, 170), '18':(1047, 170), '37':(923, 170), '22':(795, 160), '53':(694, 150), '17':(582, 160), '35':(449, 160), '69':(509, 140), '14':(622, 140), '4':(738, 140), '21':(819, 145), '19':(901, 150), '13':(984, 160), '6':(1114, 155), '9':(1028, 150), '24':(1213, 145), '3':(1090, 140), '8':(943, 140), '10':(943, 135), '5':(769, 140), '11':(909, 140), '60':(999, 145), '15':(1176, 150), '1':(1274, 150), '7':(1295, 140), '2':(1147, 135), '29':(1051, 130), '16':(983, 130), '0':(861, 140), '12':(838, 130), '40':(745, 135), '73':(1093, 155)},
        ('D2', (300, 0)) : {},
        ('D3', (530, 180)) : {},
        ('D4', (800, 320)) : {}}

def polar_to_cart(rad, ang):
    '''Takes rad in cm and ang in degrees and returns x and y coords in dm'''
    x = rad/10 * math.cos(math.radians(ang))
    y = rad/10 * math.sin(math.radians(ang))
    return x, y

def build_loc_dicts(locs_in_polar):
    drum_dict, lamp_dict = {}, {}
    for drum_data, polar_lamp_loc in locs_in_polar.items():
        drum_name, polar_drum_loc = drum_data
        drum_dict[drum_name], lamp_dict = build_loc_dict(polar_drum_loc, polar_lamp_loc, lamp_dict)
    return drum_dict, lamp_dict

def build_loc_dict(polar_drum_loc, polar_lamp_loc, lamp_dict):
    offset = polar_to_cart(*polar_drum_loc)
    for k, v in polar_lamp_loc.items():
        assert(k not in lamp_dict.keys())
        x, y = polar_to_cart(v[0], v[1])
        x += offset[0]
        y += offset[1]
        lamp_dict[k] = int(x), int(y)
    return (int(offset[0]), int(offset[1])), lamp_dict

def normalise(loc_dict):
    min_x = min(item[0] for item in loc_dict.values())
    min_y = min(item[1] for item in loc_dict.values())
    for k, v in loc_dict.items():
        loc_dict[k] = v[0] - min_x, v[1] - min_y
    return loc_dict

def list_to_C_array_string(python_list):
    string_list = str(python_list)
    replacements = (('(', '{'),
                    (')', '}'),
                    ('[', '{'),
                    (']', '}'))
    for rep in replacements:
        string_list = string_list.replace(*rep)
    return string_list

def add_equals(name, value):
    return name + ' = ' + str(value) + ';'

drum_dict, lamp_dict = build_loc_dicts(polar_coords)
cart_coords = normalise({**drum_dict, **lamp_dict})

drum_loc = [v for k, v in cart_coords.items() if k[0] == 'D']
drum_rgb = [[17,60,56],[60,55,10],[10,10,60],[60,0,0]]
lamps = {str(i) : cart_coords[str(i)] for i in range(NUM_LAMPS)}

is_relay = ()
wavelength = 1
period = 250
expiry = 50

if __name__ == '__main__':
    # for generating C array
    config_vars = {'uint16_t lamp_loc[][2]' :
                    [lamps[str(i)] for i in range(NUM_LAMPS)],
                   'uint16_t drum_loc[][2]' : drum_loc,
                   'uint8_t drum_colours[][3]' : drum_rgb,
                   'uint8_t period' : period,
                   'uint8_t wl' : wavelength,
                   'uint8_t expiry' : expiry}
    config_header = ''
    for name, value in config_vars.items():
        if type(value) is list:
            C_var = add_equals(name, list_to_C_array_string(value))
        else:
            C_var = add_equals(name, value)
        config_header += C_var + '\n'

    with open('config.h', 'w') as f:
        print(config_header, file=f)

