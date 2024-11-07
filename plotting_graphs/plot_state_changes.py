import matplotlib.pyplot as plt
import pandas as pd

reader_log = pd.read_csv('logs/reader_log.txt', names=['Time', 'State', 'Page'])
writer_log = pd.read_csv('logs/writer_log.txt', names=['Time', 'State', 'Page'])

start_time = min(reader_log['Time'].min(), writer_log['Time'].min())
reader_log['Time'] = (reader_log['Time'] - start_time) / 1000
writer_log['Time'] = (writer_log['Time'] - start_time) / 1000

plt.figure(figsize=(14, 7))
for state in reader_log['State'].unique():
    state_data = reader_log[reader_log['State'] == state]
    plt.plot(state_data['Time'], state_data['Page'], linestyle='-', marker='o', label=f'Reader {state}')

for state in writer_log['State'].unique():
    state_data = writer_log[writer_log['State'] == state]
    plt.plot(state_data['Time'], state_data['Page'], linestyle='--', marker='x', label=f'Writer {state}')

plt.xlabel('Time (seconds)')
plt.ylabel('Buffer Memory Page')
plt.title('State Changes Over Time for Reader and Writer')
plt.legend()
plt.grid(True)
plt.show()
