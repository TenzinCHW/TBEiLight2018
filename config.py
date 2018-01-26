
drum_loc = [[0,0],[0,30.6],[20,10.3],[32,6.5]]
drum_rgb = [[123,243,10],[0,50,100],[100,100,0],[255,0,0]]
drums = {
    '1': {
        'loc': drum_loc[0], 
        'isRelay': False
    },
    '2': { 
        'loc': drum_loc[1],
        'isRelay': True
    },
    '3': {
        'loc': drum_loc[2],
        'isRelay': False
    }
}
lamps = {
    '1': {
        'loc': [0,0], 
        'isRelay': False
    },
    '2': { 
        'loc': [0,20],
        'isRelay': True
    },
    '3': {
        'loc': [29,11],
        'isRelay': False
    }
}
wavelength = 100
period = 20
expiry = 3  
