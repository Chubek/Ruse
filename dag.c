#include <stdlib.h>

typedef struct DAGNode {
  IROpcode opcode;
  SSAOperand result;
  IROperand operand1;
  IROperand operand2;
  struct DAGNode *left;
  struct DAGNode *right;
} DAGNode;

typedef enum {
  INST_DAG_NODE,
  INST_DAG_PHI,
} DAGInstructionKind;

typedef struct {
  DAGInstructionKind kind;
  union {
    PhiInstruction phiDAGInstruction;
    DAGNode *dagNode;
  };
} DAGInstruction;

DAGNode *createDAGNode(IROpcode opcode, SSAOperand result, IROperand operand1,
                       IROperand operand2, DAGNode *left, DAGNode *right) {
  DAGNode *node = (DAGNode *)zalloc(sizeof(DAGNode));
  node->opcode = opcode;
  node->result = result;
  node->operand1 = operand1;
  node->operand2 = operand2;
  node->left = left;
  node->right = right;
  return node;
}

DAGInstruction createDAGInstruction(DAGInstructionKind kind,
                                    PhiInstruction phiInstr, DAGNode *dagNode) {
  DAGInstruction instruction;
  instruction.kind = kind;

  if (kind == INST_DAG_PHI) {
    instruction.phiInstruction = phiInstr;
  } else {
    instruction.dagNode = dagNode;
  }

  return instruction;
}

IRFunction createDAGIrFunction(InstructionBlock *instructionBlocks,
                               CFGBlock *entryCFGBlock, int numBlocks) {
  IRFunction irFunc;
  irFunc.instructionBlocks = instructionBlocks;
  irFunc.entryCFGBlock = entryCFGBlock;
  irFunc.numBlocks = numBlocks;
  return irFunc;
}

void optimizeRemoveDeadCode(DAGNode *root) {
  if (root == NULL) {
    return;
  }

  if (root->opcode == IR_NULL) {
    // Null node, remove it
    root = NULL;
    return;
  }

  nullSequenceOptimization(root->left);
  nullSequenceOptimization(root->right);

  // Check if both children are null, and the node itself is not a PHI node
  if (root->opcode != IR_PHI && root->left == NULL && root->right == NULL) {
    // Null sequence, replace this node with a null node
    root->opcode = IR_NULL;
    root->result.irValue = NULL;
  }
}
