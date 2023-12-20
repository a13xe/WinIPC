import pandas as pd
import matplotlib.pyplot as plt

# Read the log files
writer_log = pd.read_csv('writer_log.txt', names=['Time', 'Action', 'Page'])
reader_log = pd.read_csv('reader_log.txt', names=['Time', 'Action', 'Page'])

# Convert time to seconds
writer_log['Time'] = writer_log['Time'] / 1000.0
reader_log['Time'] = reader_log['Time'] / 1000.0

# Plotting the occupancy of buffer memory pages
plt.figure(figsize=(12, 6))
plt.scatter(writer_log['Time'], writer_log['Page'], alpha=0.6, label='Writer', marker='o')
plt.scatter(reader_log['Time'], reader_log['Page'], alpha=0.6, label='Reader', marker='x')
plt.xlabel('Time (seconds)')
plt.ylabel('Buffer Memory Page')
plt.title('Buffer Memory Page Occupancy Over Time')
plt.legend()
plt.show()
