import unittest
import os

import network
import py2pdevs.params as params


class BaseTest(unittest.TestCase):
    GET_CMDLM_METHOD = params.get_cmdline_options
    MOCK_PARAMS = {'finalization_script': '',
                   'reading_backend': 'Python',
                   'logging_backend': 'Default',
                   'params': '',
                   'ti': 0.0,
                   'tf': 10.0,
                   'max_steps': 100000,
                   'samplingPeriodForPlots': 0.1,
                   'debugLevel':999999999,
                   'logLevel':999999999}
                   
    
    def tearDown(self):
        params.get_cmdline_options = BaseTest.GET_CMDLM_METHOD
        if self.remove_logs:
            os.system('rm -rf pdevs.log')
            os.system('rm -rf pdevs_run0.log')
#         os.system('rm -rf *.h5')
    
    def setUp(self):
        # TODO: refactor into base test class.
        BaseTest.GET_CMDLM_METHOD = params.get_cmdline_options
        params.get_cmdline_options = lambda: BaseTest.MOCK_PARAMS    
        self.remove_logs = True
        
