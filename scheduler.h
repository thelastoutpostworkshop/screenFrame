// Class for the Task scheduler
//
class Task {
public:
  typedef void (*taskCallback)(void);

  // Default constructor
  Task() : callback(nullptr), interval(0), lastRun(0) {}

  Task(taskCallback cb, unsigned long intervalMillis) : callback(cb), interval(intervalMillis) {
    lastRun = millis();
  }

  void runIfDue() {
    unsigned long now = millis();
    if (now - lastRun >= interval) {
      lastRun = now;
      callback();
    }
  }

private:
  taskCallback callback;
  unsigned long interval;
  unsigned long lastRun;
};

class TaskScheduler {
public:
  TaskScheduler() {
    taskCount = 0;  // Initialize taskCount
  }

  void addTask(Task::taskCallback cb, unsigned long intervalMillis) {
    if (taskCount < MAX_TASKS) {
      tasks[taskCount++] = Task(cb, intervalMillis);
    }
  }

  void runTasks() {
    for (int i = 0; i < taskCount; ++i) {
      tasks[i].runIfDue();
    }
  }

private:
  static const int MAX_TASKS = 10; // Max number of tasks that can be scheduled.
  Task tasks[MAX_TASKS];
  int taskCount;
};
