struct Node {
    int key;
    Node *next;
};

struct Queue {
    Node *head;
    Node *tail;
};

void initQueue(Queue *&q) {
    q->head = q->tail = nullptr;
}

bool isEmpty(Queue *q) {
    return q->head = nullptr;
}

bool enQueue(Queue *&q, int k) {
    Node *p = new Node;
    if (p == nullptr) {
        return false;
    }
    p->key = k;
    p->next = nullptr;
    if (isEmpty(q)) {
        q->head = q->tail = p;
        return true;
    }
    (q->tail)->next = p;
    q->tail = p;
    return true;
}

bool deQueue(Queue *&q) {
    if (!isEmpty(q)) {
        Node *p = q->head;
        q->head = p->next;
        p->next = nullptr;
        delete p;
        if (isEmpty(q)) {
            q->tail = nullptr;
        }
        return true;
    }
    return false;
}