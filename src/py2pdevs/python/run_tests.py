import os
import sys
import unittest

sys.path.extend(['python', '../../build/lib'])


class Py2PDEVSTestSuite(object):

    SOURCE_FILES = os.listdir('python/test')
    
    @classmethod
    def build(cls):
        test_files = cls.get_all_test_files()
        return cls.get_test_suite_from(test_files)

    @classmethod
    def get_test_suite_from(cls, test_files):
        test_loader = unittest.defaultTestLoader
        test_modules = map(lambda filename: 'test.%s' % filename[:-3],
                           test_files)
        suites = map(test_loader.loadTestsFromName, test_modules) 
        test_suite = unittest.TestSuite(suites)
        return test_suite

    @classmethod
    def get_all_test_files(cls):
        return filter(lambda filename: filename.startswith('test') and\
                      filename.endswith('.py'), cls.SOURCE_FILES)

def run_tests():
    test_suite = Py2PDEVSTestSuite.build()
    runner = unittest.TextTestRunner(verbosity=2)
    runner.run(test_suite)


if __name__ == '__main__':
    run_tests()