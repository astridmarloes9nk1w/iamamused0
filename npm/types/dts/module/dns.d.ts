/**
 * @description dns 域名查询模块
 * 
 *  基础模块，引用方式：
 *  ```JavaScript
 *  var dns = require('dns');
 *  ```
 *  
 */
declare module 'dns' {
    /**
     * @description 查询给定的主机名的地址
     *      @param name 指定主机名
     *      @return 返回查询的 ip 字符串数组
     *      
     */
    function resolve(name: string): any[];

    /**
     * @description 查询给定的主机名的地址
     *      @param name 指定主机名
     *      @return 返回查询的 ip 字符串
     *      
     */
    function lookup(name: string): string;

}

