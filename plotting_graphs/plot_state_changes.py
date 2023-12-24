import pandas as pd
import matplotlib.pyplot as plt

# Load the logs into pandas DataFrames
reader_log = pd.read_csv('logs/reader_log.txt', names=['Time', 'State', 'Page'])
writer_log = pd.read_csv('logs/writer_log.txt', names=['Time', 'State', 'Page'])

# Convert the time to seconds from the start of the log
start_time = min(reader_log['Time'].min(), writer_log['Time'].min())
reader_log['Time'] = (reader_log['Time'] - start_time) / 1000
writer_log['Time'] = (writer_log['Time'] - start_time) / 1000

# Plotting state changes for reader
plt.figure(figsize=(14, 7))
for state in reader_log['State'].unique():
    state_data = reader_log[reader_log['State'] == state]
    plt.plot(state_data['Time'], state_data['Page'], linestyle='-', marker='o', label=f'Reader {state}')

# Plotting state changes for writer
for state in writer_log['State'].unique():
    state_data = writer_log[writer_log['State'] == state]
    plt.plot(state_data['Time'], state_data['Page'], linestyle='--', marker='x', label=f'Writer {state}')

plt.xlabel('Time (seconds)')
plt.ylabel('Buffer Memory Page')
plt.title('State Changes Over Time for Reader and Writer')
plt.legend()
plt.grid(True)
plt.show()
