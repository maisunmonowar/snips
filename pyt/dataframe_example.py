# %% 
import pandas as pd
import datetime
import numpy as np
# %%
# Example dataframes for testing.
dfa = pd.DataFrame({
    'time': [datetime.datetime(2020,1,1,12,0,0, tzinfo=datetime.timezone.utc), 
             datetime.datetime(2020,1,1,12,0,1, tzinfo=datetime.timezone.utc),
             datetime.datetime(2020,1,1,12,0,2, tzinfo=datetime.timezone.utc)
             ],
    'voltage': [1.0, 2.0,3],
    'current': [0.1, 0.2, .3],
    'temperature': [20, 20.1, 20.2]
})
dfb = pd.DataFrame({
    'time': [datetime.datetime(2020,1,1,12,0,0,0, tzinfo=datetime.timezone.utc), 
             datetime.datetime(2020,1,1,12,0,0,1, tzinfo=datetime.timezone.utc),
             datetime.datetime(2020,1,1,12,0,0,2, tzinfo=datetime.timezone.utc)
             ],
    'voltage': [1.1, 2.1,3.1],
    'current': [1.1, 2.2, 2.3]
})
dfa.set_index('time', inplace=True)
dfb.set_index('time', inplace=True)
# %%
# Example of concatenating by axis=0 (stacking rows).
# Since 'time' is the index, concat aligns rows by time index.
# Rows with the same time index will be merged, rows with different times are stacked.
# In this case, index 0 matches (both at 12:00:00) and will stack columns together.
result_concat_axis0 = pd.concat([dfa, dfb], axis=0)
print(result_concat_axis0.head())
# Expected output (10 rows total, with voltage/current from both stacked at same times):
#                                      voltage  current  temperature  voltage  current
# 2020-01-01 12:00:00+00:00            1.0      0.1      20.0        1.1      1.1
# 2020-01-01 12:00:00+00:00            NaN      NaN      NaN         NaN      NaN
# 2020-01-01 12:00:01+00:00            2.0      0.2      20.0        NaN      NaN
# 2020-01-01 12:00:00.000001+00:00     NaN      NaN      NaN         2.1      2.2
# ... (10 rows total when duplicated indices stack)

# %%
# Example of concatenating by axis=1 (combining columns side-by-side).
# This appends the columns from dfb to the right of dfa columns.
# With time as index, rows align by time indices. Only matching times will have values.
# Non-matching times result in NaN for the other dataframe's columns.
result_concat_axis1 = pd.concat([dfa, dfb], axis=1, keys=['dfA', 'dfB'])
print(result_concat_axis1.head())
# Expected output (time indices aligned, non-matching times get NaN):
#                                      dfA                         dfB
#                               voltage current temperature    voltage current
# 2020-01-01 12:00:00+00:00       1.0    0.1      20.0          1.1     1.1
# 2020-01-01 12:00:01+00:00       2.0    0.2      20.0          NaN     NaN
# 2020-01-01 12:00:02+00:00       3.0    0.3      20.1          NaN     NaN
# 2020-01-01 12:00:03+00:00       4.0    0.4      20.1          NaN     NaN
# 2020-01-01 12:00:04+00:00       5.0    0.5      20.2          NaN     NaN
# 2020-01-01 12:00:00.000001+00:00 NaN    NaN      NaN          2.1     2.2
# ... (10 rows total)

# %%
# Example of join/merge on time index.
# Since time is already the index, use join() which merges on index automatically.
# Inner join: keeps only rows where time indices match (only at 12:00:00).
# Left join: keeps all rows from dfa, fills NaN for non-matching dfb times.
result_join_inner = dfa.join(dfb, how='inner', lsuffix='_a', rsuffix='_b')
print(result_join_inner.head())
# Expected output (only 1 row matches at 2020-01-01 12:00:00+00:00):
#                                      voltage_a  current_a  temperature  voltage_b  current_b
# 2020-01-01 12:00:00+00:00            1.0        0.1        20.0        1.1        1.1
# %%
# Left join example: keeps all dfa times, fills NaN for dfb when times don't match
result_join_left = dfa.join(dfb, how='left', lsuffix='_a', rsuffix='_b')
print(result_join_left.head())
# Expected output (5 rows from dfa, only first row has matching dfb data):
#                                      voltage_a  current_a  temperature  voltage_b  current_b
# 2020-01-01 12:00:00+00:00            1.0        0.1        20.0        1.1        1.1
# 2020-01-01 12:00:01+00:00            2.0        0.2        20.0        NaN        NaN
# 2020-01-01 12:00:02+00:00            3.0        0.3        20.1        NaN        NaN
# 2020-01-01 12:00:03+00:00            4.0        0.4        20.1        NaN        NaN
# 2020-01-01 12:00:04+00:00            5.0        0.5        20.2        NaN        NaN

# %% 
# Example of resampling by time.
# Since time is already the index, we can directly use resample().
# Resample dfa to 2-second intervals and compute mean values.
# This groups timepoints into buckets and aggregates them.
result_resample_2s = dfa.resample('2s').mean()
print(result_resample_2s.head())
# Expected output (2-second intervals with mean of voltage, current, temperature):
#                                      voltage  current  temperature
# 2020-01-01 12:00:00+00:00            1.5      0.15     20.0
# 2020-01-01 12:00:02+00:00            3.5      0.35     20.05
# 2020-01-01 12:00:04+00:00            5.0      0.5      20.2
# %%
# Resample with different aggregation: last value in each interval
result_resample_last = dfa.resample('2s').last()
print(result_resample_last.head())
# Expected output (last value in each 2-second bucket):
#                                      voltage  current  temperature
# 2020-01-01 12:00:00+00:00            2.0      0.2      20.0
# 2020-01-01 12:00:02+00:00            4.0      0.4      20.1
# 2020-01-01 12:00:04+00:00            5.0      0.5      20.2

# %%
# BONUS: Additional useful merge/concat patterns

# Example 1: Outer join (union of all times from both dataframes)
# Keeps all rows from both dataframes, filling NaN where data is missing
result_join_outer = dfa.join(dfb, how='outer', lsuffix='_a', rsuffix='_b')
print(result_join_outer.head())
# Expected output (10 rows: all unique times from both dataframes):
#                                      voltage_a  current_a  temperature  voltage_b  current_b
# 2020-01-01 12:00:00+00:00            1.0        0.1        20.0        1.1        1.1
# 2020-01-01 12:00:00.000001+00:00     NaN        NaN        NaN         2.1        2.2
# ... (10 rows total with all unique times)

# Example 2: Concatenating with keys to create MultiIndex (useful for tracking sources)
result_concat_keys = pd.concat([dfa, dfb], axis=0, keys=['sensor_a', 'sensor_b'])
print(result_concat_keys.head())
# Creates a MultiIndex with source labels in the index.
# Expected output (10 rows with MultiIndex):
#                                sensor   time
#                                voltage  current  temperature
# sensor_a  2020-01-01 12:00:00      1.0      0.1      20.0
#           2020-01-01 12:00:01      2.0      0.2      20.0
# ...
# sensor_b  2020-01-01 12:00:00      1.1      1.1      NaN

# Example 3: Interpolate missing values after outer join
# Useful when you have time series from different sources and want to fill gaps
result_interpolated = dfa.join(dfb, how='outer', lsuffix='_a', rsuffix='_b').sort_index()
result_interpolated = result_interpolated.interpolate(method='time')
print(result_interpolated.head())
# This fills NaN values by linear interpolation between surrounding time points,
# useful for combining sensors with different sampling rates.
# %%
# so if I want to merge high freq data with standard
# freq
result_resample_2s_b = dfb.resample('2s').mean()
resampled_join = pd.concat([result_resample_2s, result_resample_2s_b], axis=0)
# the problem with this approach is, now you have 
# 2 values at the same time.

# %%
resampled_mergee = result_resample_2s.join(result_resample_2s_b, how='outer', lsuffix='_a', rsuffix='_b')
resampled_mergee['voltage_final'] = resampled_mergee['voltage_a'].combine_first(resampled_mergee['voltage_b'])
print(resampled_mergee.head())

# %%
what_matters = resampled_mergee[resampled_mergee['voltage_final'] > 2]
print(what_matters.head())
