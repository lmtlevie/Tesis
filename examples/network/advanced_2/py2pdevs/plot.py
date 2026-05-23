import matplotlib
matplotlib.use('GTKAgg')
import matplotlib.pyplot as plt


def plot_cwnd(reader):
    time1, values1 = reader.read_variable('Sender1_cwnd')
    time2, values2 = reader.read_variable('Sender2_cwnd')
    
    plt.plot(time1, values1, color="blue", linewidth=2.5, linestyle="-", label="Sender1 CWND")
    plt.plot(time2, values2, color="red", linewidth=2.5, linestyle="-", label="Sender2 CWND")

    plt.xlabel('Time [s]')
    plt.ylabel('CWND [B]')
    plt.legend(loc='upper left')

def plot_drops(reader):
    time1, values1 = reader.read_variable('Router1_tail_drops0')
    time2, values2 = reader.read_variable('Router1_red_drops0')
    
    plt.plot(time1, values1, color="blue", linewidth=2.5, marker="x", linestyle='None', label="Router1 tail drops")
    plt.plot(time2, values2, color="red", linewidth=2.5, marker="x", linestyle='None', label="Router1 RED drops")

    plt.xlabel('Time [s]')
    plt.ylabel('Queue size [b]')
    plt.legend(loc='upper left')

def plot_qsize(reader):
    time1, values1 = reader.read_variable('Router1_qsize0')
    
    plt.plot(time1, values1, color="blue", linewidth=2.5, linestyle="-", label="Router1 queue0 size")

    plt.xlabel('Time [s]')
    plt.ylabel('Queue size [b]')
    plt.legend(loc='upper left')

def make_plots(model):
    reader = model.get_h5_reader()

    plt.title('Advanced network example #2')

    plt.subplot(3, 1, 1)
    plot_cwnd(reader)
    
    plt.subplot(3, 1, 2)
    plot_qsize(reader)

    plt.subplot(3, 1, 3)
    plot_drops(reader)

    plt.show()

if __name__ == '__main__':
    make_plots()
