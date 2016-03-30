
### MOS_Break()

##### Description

Suspend task execution for one cycle. The task will be resumed with the next
call of the loop function.

##### Syntax/Example

```C++
MOS_Break(tcb);
```

##### Parameters

| Parameter | Description |
|-----------|------------ |
| tcb | Task control block, passed to the task as first parameter (PTCB) |

##### Returns
none

##### See also
-

### MOS_Continue()

##### Description

Continue the task execution at the previous suspended program position.

##### Syntax/Example

```C++
MOS_Continue(tcb);
```

##### Parameters

| Parameter | Description |
|-----------|------------ |
| tcb | Task control block, passed to the task as first parameter (PTCB) |

##### Returns
none

##### See also
MOS_Break()
