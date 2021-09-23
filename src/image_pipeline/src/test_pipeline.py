import unittest
import pipeline

class PipelineTest(unittest.TestCase):
    def setUp(self):
        self.image = pipeline.Pipeline()

    def tearDown(self):
        del(self.image)

if __name__ == "__main__":
    unittest.main()
