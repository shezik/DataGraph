window does not move (from .. to ..), pre-existing data in the ring buffer gets pushed left to fit in a new value, so the graph appears to be moving left.

y = round(value / peakValue * graphHeight) - 1  // out of bounds drawing?