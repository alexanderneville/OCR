#!/usr/bin/env python

import pipeline

image = pipeline.Pipeline()
image.load_file("landscape.png")
image.resize(1.5)
image.save_to_file("processed/large.png")
