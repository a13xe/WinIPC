import pandas as pd
import matplotlib.pyplot as plt

# Read the log files
writer_log = pd.read_csv('writer_log.txt', names=['Time', 'State', 'Page'])
reader_log = pd.read_csv('reader_log.txt', names=['Time', 'State', 'Page'])

# Convert time to seconds
writer_log['Time'] = writer_log['Time'] / 1000.0
reader_log['Time'] = reader_log['Time'] / 1000.0

# Plotting state changes
plt.figure(figsize=(12, 6))
plt.plot(writer_log['Time'], writer_log['State'], label='Writer State')
plt.plot(reader_log['Time'], reader_log['State'], label='Reader State')
plt.xlabel('Time (seconds)')
plt.ylabel('State')
plt.title('State Changes Over Time for Reader and Writer')
plt.legend()
plt.show()
