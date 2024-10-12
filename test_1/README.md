## Symbol Table

| Function Name | Location Address | Comments            |
|---------------|-----------------|---------------------|
| flushInput    | 0x08049256      |                     |
| countW        | 0x08049274      |                     |
| initSecret    | 0x080492e4      |                     |
| getWord       | 0x080493b8      |                     |
| eraseWord     | 0x080493bc      |                     |
| exist         | 0x080493e4      |                     |
| match         | 0x08049489      |                     |
| play          | 0x08049619      |                     |
| askChoice     | 0x08049712      |                     |
| main          | 0x080497dd      |                     |

## Play Function Call Table

| Function Called | Location Address | Comments            |
|-----------------|-----------------|---------------------|
| strlen          | N/A             | Standard library function for string length calculation |
| initSecret      | 0x080492e4      |                     |
| getWord         | 0x080493b8      |                     |
| exist           | 0x080493e4      |                     |
| match           | 0x08049489      |                     |

## AskChoice Function Call Table

| Function Called | Location Address | Comments            |
|-----------------|-----------------|---------------------|
| getchar         | N/A             | Standard library function for character input          |
| flushInput      | 0x08049256      |                     |