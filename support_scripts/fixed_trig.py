## Fixed Trig

from math import *


def build_sine_data(input):
    amin = 0.0
    N = input['N']
    cutoffs = input['cutoffs']
    counts = input['counts']
    values = []

    cmin = amin
    for i in xrange(N):
        cmax = cutoffs[i]
        delta = (cmax - cmin) / counts[i]
        segment_values = []
        for j in xrange(counts[i]+1):
            ang = cmin + j * delta
            v = sin(ang * pi / 180)
            x = int(round(0x8000 * v))
            segment_values.append(x)
        values.append(segment_values)
    input['values'] = values
    return input


def build_arcsine_data(input):
    amin = 0.0
    N = input['N']
    cutoffs = input['cutoffs']
    counts = input['counts']
    values = []

    cmin = amin
    for i in xrange(N):
        cmax = cutoffs[i]
        delta = (cmax - cmin) / counts[i]
        segment_values = []
        for j in xrange(counts[i]+1):
            x = cmin + j * delta
            v = asin(x) * 180 / pi
            ang = int(round(0x8000 * v))
            segment_values.append(ang)
        values.append(segment_values)
        cmin = cmax
    input['values'] = values
    return input



def lookup_value(data, angle):
    ## Step 1: Find which segment this angle falls in
    cmin = 0.0
    segment = 0
    while angle > data['cutoffs'][segment] and segment < data['N']:
        cmin = data['cutoffs'][segment]
        segment += 1
    cmax = data['cutoffs'][segment]

    ## now segment should index to the segment that contains angle,
    ##    and cmin and cmax should be set for this interval

    ## Step 2: Convert angle to an index into this segment
    count = data['counts'][segment]
    index = count * (angle - cmin) / (cmax - cmin)

    ## Step 3: lookup the values, and interpolate
    idx_1 = int(floor(index))
    idx_2 = int(ceil(index))

    value_1 = data['values'][segment][idx_1]
    value_2 = data['values'][segment][idx_2]

    result = int(round(value_1 + (value_2 - value_1) * (index - idx_1)))
    return result


def test_sine(data):
    err_sum = 0
    err_cnt = 0
    max_val = 0
    max_idx = 0
    with open("sine_test.csv", "w") as f:
        f.write("angle,y,yhat,error\n")
        for i in xrange(90 * 100 + 1):
            angle = i * 0.01
            yhat = lookup_value(data, angle)
            y = int(round(sin(angle * pi / 180.0) * 0x8000))
            err = yhat - y
            err_sum += err
            err_cnt += 1
            if abs(err) > abs(max_val):
                max_val = err
                max_idx = angle
            f.write("%0.3f,%d,%d,%d\n" % (angle, y, yhat, err))
    print 'Sine Summary:'
    print '====================='
    print 'mean err:', (float(err_sum) / err_cnt) / 0x8000
    print 'max err:', float(max_val) / 0x8000
    print 'at angle =', max_idx
    print


def test_arcsine(data):
    err_sum = 0
    err_cnt = 0
    max_val = 0
    max_idx = 0
    with open("arcsine_test.csv", "w") as f:
        f.write("x,y,yhat,error\n")
        granularity = 100000
        for i in xrange(granularity + 1):
            x = i * 1.0 / granularity # + 0.9 ## DEBUG!!!!  take out the + 0.9
            yhat = lookup_value(data, x)
            y = int(round(asin(x) * 180 / pi * 0x8000))
            err = yhat - y
            err_sum += err
            err_cnt += 1
            if abs(err) > abs(max_val):
                max_val = err
                max_idx = x
            f.write("%0.3f,%d,%d,%d\n" % (x, y, yhat, err))

    print 'ArcSine Summary:'
    print '====================='
    print 'mean err:', (float(err_sum) / err_cnt) / 0x8000
    print 'max err:', float(max_val) / 0x8000
    print 'at x =', max_idx
    print


def fwrite(f, txt = ''):
    f.write(txt + '\n')


def generate_header(data, prefix):
    filename = prefix + "_data.h"
    headername = filename.upper()
    Nstr = str(data['N'])

    with open('../arbotix/libraries/Fixed/' + filename, 'w') as f:
        fwrite(f, '#ifndef ' + headername)
        fwrite(f, '#define ' + headername)
        fwrite(f)
        fwrite(f, 'const float ' + prefix + '_max = %0.2f;' % data['max'])
        fwrite(f, 'const int ' + prefix + '_N = %d' % data['N'] + ';')
        fwrite(f, 'const float ' + prefix + '_cutoffs[] = {' + ','.join(map(str, data['cutoffs'])) + '};')
        fwrite(f, 'const int ' + prefix + '_counts[] = {' + ','.join(map(str, data['counts'])) + '};')

        N = len(data['values'])
        for i in range(N):
            value_list = data['values'][i]
            fwrite(f, 'const long int ' + prefix + '_values_%d' % i + '[] = {' + ','.join(map(str, value_list)) + '};')

        fwrite(f, 'const long int* ' + prefix + '_values[] = {' +
                  ','.join(map(lambda(x): prefix + '_values_' + str(x), range(N))) + '};')
        fwrite(f)
        fwrite(f, '#endif')


sine_data_input = dict(max=90.0, N=1, cutoffs=[90.0], counts=[180])
sine_data = build_sine_data(sine_data_input)

arcsine_data_input = dict(max=1.0, N=4, cutoffs=[0.9, 0.99, 0.999, 1.0], counts=[90,50,50,50])
arcsine_data = build_arcsine_data(arcsine_data_input)

test_sine(sine_data)
test_arcsine(arcsine_data)

generate_header(sine_data, 'sine')
generate_header(arcsine_data, 'arcsine')

