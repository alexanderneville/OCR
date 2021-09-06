#!/usr/bin/env python

import pipeline

images = []

for i in range(1):
    images.append(pipeline.Pipeline())
    images[i].load_file("landscape.png")

images[0].switch_channel_num()
# images[1].resize(0.5)
# images[2].resize(1.5)
# images[3].process(2, 7, 0.1)
# images[4].process(2, 17, 0.9)
# images[5].invert()

for i in range(1):
    images[i].save_to_file("processed/test.png")
