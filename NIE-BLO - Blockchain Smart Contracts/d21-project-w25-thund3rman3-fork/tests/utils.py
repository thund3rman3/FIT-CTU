from wake.testing import *
from pytypes.contracts.D21 import D21
import logging

logger = logging.getLogger(__name__)
logger.setLevel(logging.DEBUG) # logger.info(f"Swapped {amount}")

def end_voting(contract):
    # End voting
    time = contract.getRemainingTime() 
    logger.info(f"Time remaining: {time}")
    default_chain.mine(lambda t: t + 604800)  # Advance time by 7 days
    time = contract.getRemainingTime() 
    logger.info(f"Time remaining: {time}")
