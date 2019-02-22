import math

Drums = {'D1':(0,0), 'D2':(300, 0), 'D3':(530, 180), 'D4':(800, 320)}

D1 = {'71':(173, 0), '79':(323, 10), '44':(504, 0), '66':(597, 0), '41':(763, 0), '46':(692, 10), '78':(589, 10), '54':(403, 20), '75':(238, 40), '48':(123, 45), '58':(184, 70), '67':(334, 45), '68':(489, 30), '63':(636, 30), '76':(761, 20), '52':(832, 40), '64':(720, 40), '72':(557, 40), '70':(477, 45), '42':(371, 55), '45':(264, 60), '39':(223, 80), '31':(110, 120), '61':(196, 100), '34':(258, 115), '43':(337, 125), '50':(309, 90), '59':(365, 80), '25':(349, 60), '62':(482, 45), '20':(531, 40), '47':(578, 30), '55':(701, 25), '65':(804, 20), '49':(496, 60), '26':(478, 80), '51':(447, 90), '77':(504, 100), '33':(406, 95), '57':(508, 120), '27':(372, 180), '30':(496, 180), '38':(613, 180), '23':(739, 180), '32':(830, 180), '36':(915, 190), '74':(989, 180), '56':(1092, 185), '28':(1161, 170), '18':(1047, 170), '37':(923, 170), '22':(795, 160), '53':(694, 150), '17':(582, 160), '35':(449, 160), '69':(509, 140), '14':(622, 140), '4':(738, 140), '21':(819, 145), '19':(901, 150), '13':(984, 160), '6':(1114, 155), '9':(1028, 150), '24':(1213, 145), '3':(1090, 140), '8':(943, 140), '10':(943, 135), '5':(769, 140), '11':(909, 140), '60':(999, 145), '15':(1176, 150), '1':(1274, 150), '7':(1295, 140), '2':(1147, 135), '29':(1051, 130), '16':(983, 130), '0':(861, 140), '12':(838, 130), '40':(745, 135), '73':(1093, 155)}

def polar_to_cart(rad, ang):
    '''Takes rad in cm and ang in degrees and returns x and y coords in dm'''
    x = rad/10 * math.cos(math.radians(ang))
    y = rad/10 * math.sin(math.radians(ang))
    return x, y

def build_loc_dict(current_dict, loc_in_polar, drums, drum_name):
    offset = polar_to_cart(*drums[drum_name])
    current_dict[drum_name] = int(offset[0]), int(offset[1])
    for k, v in loc_in_polar.items():
        assert(k not in current_dict.keys())
        x, y = polar_to_cart(v[0], v[1])
        x += offset[0]
        y += offset[1]
        current_dict[k] = int(x), int(y)
    return current_dict

def normalise(loc_dict):
    min_x = min(item[0] for item in loc_dict.values())
    min_y = min(item[1] for item in loc_dict.values())
    for k, v in loc_dict.items():
        loc_dict[k] = v[0] - min_x, v[1] - min_y
    return loc_dict

if __name__ == '__main__':
    current = build_loc_dict({}, D1, Drums, 'D1')
    #r, a = Drums['D2']
    #offset = polar_to_cart(r, a)
    #current = build_loc_dict(current, D2, offset)
    #r, a = Drums['D3']
    #offset = polar_to_cart(r, a)
    #current = build_loc_dict(current, D3, offset)
    #r, a = Drums['D4']
    #offset = polar_to_cart(r, a)
    #current = build_loc_dict(current, D4, offset)
    current = normalise(current)
    print(current)
