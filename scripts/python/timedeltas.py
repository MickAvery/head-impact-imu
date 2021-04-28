#%% [markdown]
# Import packages

#%%
import pandas as pd
import matplotlib.pyplot as plt
from datetime import datetime as dt
from datetime import timedelta as tdelta

#%%

df = pd.read_csv('./timedeltas.csv')
df['EXPECTED'] = pd.to_datetime(df['EXPECTED'])
df['ACTUAL'] = pd.to_datetime(df['ACTUAL'])
df['DELTA'] = pd.to_timedelta(df['DELTA'])

#%%

#df['DELTA'].astype('timedelta64[ms]')
plt.style.use('seaborn')

# plt.plot(df['EXPECTED'].dt.strftime('%H:%M:%S'), df['DELTA'].astype('timedelta64[ms]'))
# plt.show()

x = df['EXPECTED']
y = df['DELTA'].astype('timedelta64[ms]')-19.0

plt.figure(figsize=(12,5))
plt.plot(x, y)
plt.gcf().autofmt_xdate()
plt.title('nRF52832 RTC Drift Over Time (minus minimum drift due to communication delay)')
plt.xlabel('Time')
plt.ylabel('Time Diff. (RTC vs Reference)')
plt.ylim(0, 500)
plt.text(x.iloc[-1], y.iloc[-1], 'time delta={}'.format(y.iloc[-1]))
# plt.show()
plt.savefig('drift.png')