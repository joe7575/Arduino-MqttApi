
### MOS_Break()

##### Description

Suspend task execution for one cycle. The task will be resumed with the next
call of the loop function.

<br>
##### Syntax/Example

```C++
MOS_Break(tcb);
```

##### Parameters

| Parameter |Description  |
|-----------|------------ |
| tcb | Task control block, passed to the task as first parameter (PTCB) |

<br>
##### Returns
none

<br>
##### See also
-

------------------------------------------------

### MOS_Continue()

##### Description

Continue the task execution at the previous suspended program position.

<br>
##### Syntax/Example

```C++
MOS_Continue(tcb);
```

##### Parameters

| Parameter |Description  |
|-----------|------------ |
| tcb | Task control block, passed to the task as first parameter (PTCB) |

<br>
##### Returns
none

<br>
##### See also
MOS_Break

------------------------------------------------
