import game_algorithms_py as ga
import numpy as np

def test_tictactoe_board():
    print("=== Testing TicTacToeBoard ===")
    
    # Test basic board creation
    board = ga.TicTacToeBoard()
    print(f"Empty board:\n{board}")
    print(f"Current player: {board.getCurrentPlayer()}")
    print(f"Is terminal: {board.isTerminal()}")
    print(f"Possible actions: {board.getPossibleActions()}")
    
    # Test making moves
    board.makeMove(4)  # Center
    print(f"After move 4:\n{board}")
    board.makeMove(0)  # Top-left
    print(f"After move 0:\n{board}")
    
    # Test board transformations
    board_copy = board.copy()
    board_copy.rotateClockwise()
    print(f"After rotation:\n{board_copy}")
    
    # Test normal form
    normal = board.getNormalForm()
    print(f"Normal form:\n{normal}")
    

def test_tictactoe_node():
    print("\n=== Testing TicTacToeNode ===")
    
    node = ga.TicTacToeNode()
    print(f"Node type: {node.getType()}")
    print(f"Node type string: {node.getTypeString()}")
    print(f"Current player: {node.getCurrentPlayer()}")
    print(f"Legal actions: {list(node.getLegalActions())}")
    print(f"InfoSet key string: {node.getInfoSetKeyString()}")
    print(f"InfoSet key int: {node.getInfoSetKeyInt()}")
    
    # Apply action and get next node
    next_node = node.applyAction(4)
    print(f"After action 4 - Current player: {next_node.getCurrentPlayer()}")


def test_ttt_invariant():
    print("\n=== Testing TTTInvariant ===")
    
    base_node = ga.TicTacToeNode()
    invariant = ga.TTTInvariant(base_node)
    
    print(f"Invariant type: {invariant.getType()}")
    print(f"Unique actions: {list(invariant.getLegalActions())}")
    
    # Compare with regular node
    regular_actions = list(base_node.getLegalActions())
    invariant_actions = list(invariant.getLegalActions())
    print(f"Regular actions: {len(regular_actions)}, Invariant actions: {len(invariant_actions)}")


def test_infoset():
    print("\n=== Testing InfoSet ===")
    
    infoset = ga.InfoSet(3)  # 3 actions
    print(f"Initial regret sum: {list(infoset.getRegretSum())}")
    print(f"Initial strategy: {list(infoset.getRegretSumStrategy())}")
    
    # Set some regrets
    infoset.setInstantRegret(0, 2.0)
    infoset.setInstantRegret(1, -1.0)
    infoset.setInstantRegret(2, 0.5)
    
    print(f"Instant regrets: {list(infoset.getInstantRegret())}")
    
    # Accumulate regrets
    infoset.accumulateRegret(1.0)
    print(f"After accumulation: {list(infoset.getRegretSum())}")
    print(f"Updated strategy: {list(infoset.getRegretSumStrategy())}")


def test_infoset_maps():
    print("\n=== Testing InfoSetMaps ===")
    
    # Test string map
    str_map = ga.InfoSetMapStr()  # simple dict()
    str_map["test_key"] = ga.InfoSet(2)
    print(f"String map size: {len(str_map)}")
    print("String map contains 'test_key':", "test_key" in str_map)
    
    # Test int map
    int_map = ga.InfoSetMapStr()  # simple dict()
    int_map[12345] = ga.InfoSet(3)
    print(f"Int map size: {len(int_map)}")
    print("Int map contains 12345:", 12345 in int_map)


def test_strategy_utils():
    print("\n=== Testing Strategy Utils ===")
    
    strategy = [0.1, 0.7, 0.2]
    normalized = ga.normalizeStrategy(strategy)
    print(f"Original: {strategy}")
    print(f"Normalized: {normalized}")
    print(f"Sum: {sum(normalized)}")
    
    epsilon_soft = ga.epsilonSoftStrategy(0.1, strategy)
    print(f"Epsilon-soft (ε=0.1): {epsilon_soft}")
    print(f"Sum: {sum(epsilon_soft)}")


def test_cfr_simple():
    print("\n=== Testing CFR with TicTacToe ===")
    
    # Create root node
    root = ga.TicTacToeNode()
    
    # Create CFR instance
    cfr = ga.CFRPlusStr(root)
    
    print("Initial evaluation...")
    utility = cfr.evaluate()
    print(f"Game utility for player 0: {utility}")
    
    # Run a few iterations
    print("Running CFR iterations...")
    for i in range(3):
        utility = cfr.evaluateAndUpdate()
        print(f"Iteration {i+1}, Utility: {utility:.4f}")
    
    # Get strategy info sets
    infosets = cfr.getStrategyInfoSets()
    print(f"Number of information sets {type(infosets)}: {len(infosets)}")


def test_randomizer():
    print("\n=== Testing Randomizer ===")
    
    base_node = ga.TicTacToeNode()
    randomizer = ga.RandomizerWrapNode(base_node, 0.3)  # 30% random
    
    print(f"Randomizer type: {randomizer.getType()}")
    print(f"Legal actions: {list(randomizer.getLegalActions())}")
    print(f"Probabilities: {list(randomizer.getChanceProbabilities())}")
    
    # Apply non-random choice
    non_random = randomizer.applyAction(ga.RandomizerWrapNode.NONRANDOM_CHOICE)
    print(f"Non-random choice type: {non_random.getType()}")


class SimplePyGameNode(ga.GameNode):
    """Simple Python implementation of GameNode for testing"""
    
    def __init__(self, value=0):
        super().__init__()
        self.value = value
    
    def getType(self):
        if self.value >= 10:
            return ga.GameNodeType.Terminal
        else:
            return ga.GameNodeType.Decision
    
    def getTerminalUtilities(self):
        return [float(self.value), -float(self.value)]
    
    def getCurrentPlayer(self):
        return self.value % 2
    
    def getLegalActions(self):
        return [0, 1]
    
    def applyAction(self, action):
        return SimplePyGameNode(self.value + action + 1)
    
    def getInfoSetKeyString(self):
        return f"state_{self.value}"


def test_python_gamenode():
    print("\n=== Testing Python GameNode Implementation ===")
    
    py_node = SimplePyGameNode(0)
    print(f"Python node type: {py_node.getType()}")
    print(f"Current player: {py_node.getCurrentPlayer()}")
    print(f"Legal actions: {list(py_node.getLegalActions())}")
    print(f"InfoSet key: {py_node.getInfoSetKeyString()}")
    
    # Apply action
    next_node = py_node.applyAction(1)
    print(f"Next node player: {next_node.getCurrentPlayer()}")
    
    # Create terminal node
    terminal = SimplePyGameNode(10)
    print(f"Terminal type: {terminal.getType()}")
    print(f"Terminal utilities: {list(terminal.getTerminalUtilities())}")


def run_all_tests():
    """Run all test functions"""
    try:
        test_tictactoe_board()
        test_tictactoe_node()
        test_ttt_invariant()
        test_infoset()
        test_infoset_maps()
        test_strategy_utils()
        test_cfr_simple()
        test_randomizer()
        test_python_gamenode()
        
        print("\n" + "="*50)
        print("🎉 All tests completed successfully!")
        print("="*50)
        
    except Exception as e:
        print(f"\n❌ Test failed with error: {e}")
        import traceback
        traceback.print_exc()


if __name__ == "__main__":
    print("Testing game_algorithms_py library...")
    print("="*50)
    run_all_tests()
