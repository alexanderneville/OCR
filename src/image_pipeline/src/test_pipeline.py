import unittest
import pipeline

class PipelineTest(unittest.TestCase):

    def setUp(self):

        self.image = pipeline.Pipeline()

    def tearDown(self):

        del(self.image)

    def test_instantiation(self):

        print("testing instantiation")

    def test_check_header(self):

        print("testing check_header")
        self.assertEqual(self.image.check_header("test_image.png"), 1)
        self.assertEqual(self.image.check_header("fake_image.png"), 0)

    def test_load_file(self):

        print("testing load_file")
        self.assertEqual(self.image.load_file("test_image.png"), 1)
        self.assertEqual(self.image.infile_path, "test_image.png")

    def test_save_to_file(self):

        print("testing save_to_file")
        self.assertEqual(self.image.load_file("test_image.png"), 1)
        self.assertEqual(self.image.save_to_file("test_output_dir/wrapper_test_save.png"), 1)
        self.assertEqual(self.image.check_header("test_output_dir/wrapper_test_save.png"),1)

    def test_resize_larger(self):

        print("testing resize(1.43)")
        self.assertEqual(self.image.load_file("test_image.png"), 1)
        self.assertEqual(self.image.resize(1.43), 1)
        self.assertEqual(self.image.save_to_file("test_output_dir/wrapper_test_resize_larger.png"), 1)
        self.assertEqual(self.image.check_header("test_output_dir/wrapper_test_resize_larger.png"),1)

    def test_resize_smaller(self):

        print("testing resize(0.85)")
        self.assertEqual(self.image.load_file("test_image.png"), 1)
        self.assertEqual(self.image.resize(0.85), 1)
        self.assertEqual(self.image.save_to_file("test_output_dir/wrapper_test_resize_smaller.png"), 1)
        self.assertEqual(self.image.check_header("test_output_dir/wrapper_test_resize_smaller.png"), 1)

    def test_scan_image(self):

        print("testing scan_image()")
        self.assertEqual(self.image.load_file("test_image.png"), 1)
        self.assertEqual(self.image.scan_image(), 1)
        self.assertEqual(self.image.save_to_file("test_output_dir/wrapper_test_scan_image.png"), 1)
        self.assertEqual(self.image.check_header("test_output_dir/wrapper_test_scan_image.png"), 1)

    def test_generate_dataset(self):

        print("testing generate_dataset()")
        self.assertEqual(self.image.load_file("test_image.png"), 1)
        self.assertEqual(self.image.scan_image(), 1)
        self.assertEqual(self.image.generate_dataset("test_output_dir/wrapper_dataset.txt", "test_output_dir/wrapper_sample.txt", "test_output_dir/wrapper_info.json"), 1)

    def test_convolution(self):

        print("testing convolution()")
        self.assertEqual(self.image.load_file("test_image.png"), 1)
        self.assertEqual(self.image.convolution(14, 5), 0)
        self.assertEqual(self.image.convolution(2, 7), 1)
        self.assertEqual(self.image.save_to_file("test_output_dir/wrapper_test_convolution.png"), 1)
        self.assertEqual(self.image.check_header("test_output_dir/wrapper_test_convolution.png"), 1)

    def test_invert_colours(self):

        print("testing invert_colours()")
        self.assertEqual(self.image.load_file("test_image.png"), 1)
        self.assertEqual(self.image.invert_colours(), 1)
        self.assertEqual(self.image.save_to_file("test_output_dir/wrapper_test_invert_colours.png"), 1)
        self.assertEqual(self.image.check_header("test_output_dir/wrapper_test_invert_colours.png"), 1)
        self.assertEqual(self.image.invert_colours(), 1)

    def test_switch_channel_number(self):

        print("testing switch_channel_num()")
        self.assertEqual(self.image.load_file("test_image.png"), 1)
        self.assertEqual(self.image.switch_channel_num(), 1)
        self.assertEqual(self.image.switch_channel_num(), 1)
        pass

    def test_translation(self):
        print("testing translation()")
        self.assertEqual(self.image.load_file("test_image.png"), 1)
        self.assertEqual(self.image.translate(40, 40), 1)
        self.assertEqual(self.image.translate(-80, 20), 1)
        self.assertEqual(self.image.save_to_file("test_output_dir/wrapper_test_translate.png"), 1)
        self.assertEqual(self.image.check_header("test_output_dir/wrapper_test_translate.png"), 1)

if __name__ == "__main__":
    unittest.main()
