def getstringoflamps(name, start, num_lamps):
    D = name + ' = {'
    
    for i in range(start, start+num_lamps):
        D += "'" + str(i) + "'" + ':' + '()'
        if i != start+num_lamps-1:
            D += ', '
    D += '}\n'
    return D, start + num_lamps

start = 1
D1, start = getstringoflamps('D1', start, 45)
D2, start = getstringoflamps('D2', start, 25)
D3, start = getstringoflamps('D3', start, 33)
D4, start = getstringoflamps('D4', start, 17)

filename = 'locations.txt'
with open(filename, 'w') as f:
    print(D1, file=f)
    print(D2, file=f)
    print(D3, file=f)
    print(D4, file=f)
