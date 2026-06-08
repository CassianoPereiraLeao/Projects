from dataclasses import dataclass
from typing import Union

@dataclass
class Number:
    num: int

@dataclass
class BinaryOp:
    op: str
    left: 'Node'
    right: 'Node'

Node = Union[Number, BinaryOp]

class Parser:
    def __init__(self, expr: str):
        self.expr = expr.replace(" ", "")
        self.pos = 0

    def peek(self) -> str:
        return self.expr[self.pos] if self.pos < len(self.expr) else None
    
    def consume(self) -> str:
        char = self.peek()
        self.pos += 1
        return char
    
    def parse_number(self) -> Number:
        num = ""
        while(self.peek() and self.peek().isdigit()):
            num += self.consume()
        return Number(int(num))
    
    def parse_term(self) -> BinaryOp:
        node = self.parse_number()
        while(self.peek() in ('x', '/')):
            op = self.consume()
            node = BinaryOp(op, node, self.parse_number())
        return node

    def parse_expr(self) -> BinaryOp:
        node = self.parse_term()
        while(self.peek() in ('+', '-')):
            op = self.consume()
            node = BinaryOp(op, node, self.parse_term())
        return node
    
def evaluate(expr: Node) -> int:
    if(isinstance(expr, Number)):
        return expr.num
    
    left = evaluate(expr.left)
    right = evaluate(expr.right)

    match expr.op:
        case '+': return left + right
        case '-': return left - right
        case 'x': return left * right
        case '/': return left // right

    return 0
    
if '__main__' == __name__:
    expression = str(input("Digite sua expressão\n> "))
    parser = Parser(expression)
    ast = parser.parse_expr()
    result = evaluate(ast)
    print(f"Resultado: {result}")
