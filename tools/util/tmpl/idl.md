# <%=declare.type === 'module' ? '模块' : '对象'%> <%=declare.name%>
<%=declare.doc.descript%>

<%-declare.doc.detail.join('\n')%>
<%var last_member = '';

function def_value(v)
{
    switch(v)
    {
    case 'v8::Undefined(isolate)':
        v = 'undefined';
        break;
    case 'v8::Object::New(isolate)':
        v = '{}';
        break;
    case 'v8::Array::New(isolate)':
        v = '[]';
        break;
    }
    return v;
}

function member_output(title, test){
    var has = false;
    members.forEach(function(m){
     if(test(m, declare.name)){ 
         if(!has){
             has = true;%>## <%=title%>
        <%}
        if(last_member != m.name){%>
### <%=m.name%><%
last_member = m.name;
}else{%>
--------------------------<%}%>
<%=m.doc.descript%>
```JavaScript
<%if(m.const){%><%=m.const%> <%}
if(m.static){%><%=m.static%> <%}
if(m.readonly){%><%=m.readonly%> <%}
if(m.type){%><%=m.type%> <%}%><%=declare.name == m.name ? ' new ' : declare.name + '.'%><%=m.name%><%if(m.memType == 'method'){
    var ps = '';

    if(m.params){
        m.params.forEach(function(p){
            if(ps)
                ps += ',\n                ';

            if(p.type)
                ps += p.type + ' ';
            ps += p.name;
    
            if(p.default)
                ps += ' = ' + def_value(p.default.value);
        });
    }%>(<%-ps%>)<% if(m.async){%> async<%}}else if(m.default){%> = <%=def_value(m.default.value)%><%}%>;
```
<%if(m.params){%>
调用参数:<% m.doc.params.forEach(function(p){%>
* <%=p.name%> - <%=p.descript%><%});%>
<%}%><%if(m.doc.return){%>
返回结果:
* <%=m.doc.return.descript%><%}%>

<%-m.doc.detail.join('\n')%>
<%  }});
    }
    member_output('构造函数', function(m, n){
        return m.memType == 'method' && m.name == n;
    });

    member_output('对象', function(m){
        return m.memType == 'object';
    });

    member_output('函数', function(m, n){
        return m.memType == 'method' && m.name !== n;
    });

    member_output('属性', function(m){
        return m.memType == 'prop';
    });

    member_output('常量', function(m){
        return m.memType == 'const';
    });

%>
