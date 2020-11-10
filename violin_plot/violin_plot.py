import os
from matplotlib.backends.backend_pdf import PdfPages
import matplotlib.pyplot as plt
import numpy as np

# https://matplotlib.org/3.1.1/gallery/statistics/customized_violin.html

val = np.array([1, 2, 3, 4])
print(val - 1)


def load_data(file_path):
    data = []
    with open(file_path) as f:
        for line in f.readlines():
            line = line.strip('\n')
            data.append(int(line))
    data.sort()
    print(file_path, len(data))
    return data


file = [
    '2.4000000.1604861280', '3.4000000.1604861374', '4.4000000.1604860732',
    '5.4000000.1604860633', '6.4000000.1604860546', '7.4000000.1604860453',
    '8.4000000.1604860363', '9.4000000.1604860265', '10.4000000.1604860177',
    '11.4000000.1604860044', '12.4000000.1604859942', '13.4000000.1604859763',
    '14.4000000.1604859655', '15.4000000.1604859560', '16.4000000.1604859471'
]
data = []
for file_ in file:
    data.append(load_data('1108-2/' + file_)[10:1050])


def adjacent_values(vals, q1, q3):
    upper_adjacent_value = q3 + (q3 - q1) * 1.5
    upper_adjacent_value = np.clip(upper_adjacent_value, q3, vals[-1])

    lower_adjacent_value = q1 - (q3 - q1) * 1.5
    lower_adjacent_value = np.clip(lower_adjacent_value, vals[0], q1)
    return lower_adjacent_value, upper_adjacent_value


def set_axis_style(ax, labels, x_lable_name='Sample name'):
    ax.get_xaxis().set_tick_params(direction='out')
    ax.xaxis.set_ticks_position('bottom')
    ax.set_xticks(np.arange(1, len(labels) + 1))
    ax.set_xticklabels(labels)
    ax.set_xlim(0.25, len(labels) + 0.75)
    ax.set_xlabel(x_lable_name)


def normal_data(x, values):
    data = []
    for val in x:
        data.append(val - values)
    return data


def generate_test_data():
    np.random.seed(19680801)
    data = [sorted(np.random.normal(0, std, 100)) for std in range(1, 5)]
    return data


pdf = PdfPages('cut_figure.pdf')

fig, (ax2, ax1) = plt.subplots(nrows=1,
                               ncols=2,
                               figsize=(20, 10),
                               sharey=False)

ax1.set_title('violin plot for shifting (- medians)')
# ax1.set_ylabel('Observed values')
# ax1.violinplot(data)

ax2.set_ylabel('Observed values')
ax2.set_title('violin plot for 4MB data communication')
parts = ax2.violinplot(data,
                       showmeans=False,
                       showmedians=False,
                       showextrema=False)

for pc in parts['bodies']:
    pc.set_facecolor('#D43F3A')
    pc.set_edgecolor('black')
    pc.set_alpha(1)

min_val, quartile1, medians, quartile3, max_val = np.percentile(
    data, [0, 20, 50, 80, 100], axis=1)
mean_val = []
index = 1
for each in data:
    index = 1
    mean_val.append(round(np.mean(each) / index, 2))
    index += 1
print(mean_val)
#print(np.array(min_val).shape, np.array(data).shape)
new_data = []
for index in range(len(medians)):
    new_data.append(
        normal_data(np.array(data[index]), np.array(medians[index])))
    #print(np.array(new_data).shape)
part1 = ax1.violinplot(new_data)
inds = np.arange(1, len(medians) + 1)
ax1.scatter(inds, medians - medians, marker='o', color='white', s=30, zorder=3)
ax1.vlines(inds,
           quartile1 - medians,
           quartile3 - medians,
           color='k',
           linestyle='-',
           lw=5)
ax1.set_ylim(-100, 100)

whiskers = np.array([
    adjacent_values(sorted_array, q1, q3)
    for sorted_array, q1, q3 in zip(data, quartile1, quartile3)
])
whiskersMin, whiskersMax = whiskers[:, 0], whiskers[:, 1]

inds = np.arange(1, len(medians) + 1)
ax2.scatter(inds, medians, marker='o', color='white', s=30, zorder=3)
ax2.vlines(inds, quartile1, quartile3, color='k', linestyle='-', lw=5)
ax2.vlines(inds, whiskersMin, whiskersMax, color='k', linestyle='-', lw=1)

axis_lables = '# of nodes for Full-Mesh Communication'
# set style for the axes
labels = [x.split('.')[0] for x in file]
for ax in [ax1, ax2]:
    set_axis_style(ax, labels, axis_lables)

plt.subplots_adjust(bottom=0.15, wspace=0.05)

#plt.grid()
ax2.grid()
ax1.grid()
plt.show()
#pdf.savefig()
#plt.close()
#pdf.close()
