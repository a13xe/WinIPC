import matplotlib.pyplot as plt
import pandas as pd

def process_log(file_path):
    times, actions, pages = [], [], []
    start_time = None
    with open(file_path, 'r') as file:
        for line in file:
            time, action, page = line.strip().split(', ')
            time = int(time)
            if start_time is None:
                start_time = time
            times.append((time - start_time) / 1000000)
            actions.append(action)
            pages.append(int(page))
    return pd.DataFrame({'Time': times, 'Action': actions, 'Page': pages})

reader_log = process_log('logs/reader_log.txt')
writer_log = process_log('logs/writer_log.txt')

plt.figure(figsize=(10, 5))
plt.plot(reader_log['Time'], reader_log['Page'], label='Reader')
plt.plot(writer_log['Time'], writer_log['Page'], label='Writer')
plt.xlabel('Time (seconds)')
plt.ylabel('Page Number')
plt.title('Buffer Memory Page Usage Over Time')
plt.legend()
plt.show()