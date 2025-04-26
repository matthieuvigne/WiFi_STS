const char index_html[] PROGMEM = R"rawliteral(<!DOCTYPE HTML>
<html>

<head>
    <title>Servo Driver with ESP32</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="icon" href="data:,">
    <style>
        html {
        font-family: Arial;
        font-size: 30px;
        display: inline-block;
        background: #efefef;
        color: #000000;
        text-align: center;
    }

    h2 {
        font-size: 1.4rem;
    }

    p {
        font-size: 1.0rem;
    }

    body {
        margin: 20px;
        width: calc(100% - 40px);
        padding-bottom: 25px;
    }

    #rescanButton {
        display: inline-block;
        font-size: 1.2rem;
        margin: 5px;
        padding: 0.8rem;
        border: 0;
        cursor: pointer;
        color: #fff;
        background: #4247b7;
        border-radius: 5px;
        outline: 0;

        -webkit-touch-callout: none;
        -webkit-user-select: none;
        -khtml-user-select: none;
        -moz-user-select: none;
        -ms-user-select: none;

        user-select: none;
    }

    #rescanButton:hover {
        background: #4e56ff
    }

    #rescanButton:active {
        background: #4c3b3b
    }

    #stopButton {
        display: inline-block;
        font-size: 1.2rem;
        margin: 0px 50px 0px 0px;
        padding: 0.8rem;
        border: 0;
        line-height: 21px;
        cursor: pointer;
        color: #fff;
        background: #be0101;
        border-radius: 5px;
        outline: 0;
        float:right;

        -webkit-touch-callout: none;
        -webkit-user-select: none;
        -khtml-user-select: none;
        -moz-user-select: none;
        -ms-user-select: none;

        user-select: none;
    }

    #stopButton:hover {
        background: #ff0000
    }

    #stopButton:active {
        background: #4c3b3b
    }

    button {
        display: inline-block;
        font-size: 1.0rem;
        margin: 5px;
        padding: 0.5rem;
        border: 0;
        line-height: 21px;
        cursor: pointer;
        color: #fff;
        background: #6a6a6a;
        border-radius: 5px;
        outline: 0;
        width: 100px

        -webkit-touch-callout: none;
        -webkit-user-select: none;
        -khtml-user-select: none;
        -moz-user-select: none;
        -ms-user-select: none;

        user-select: none;
    }

    label {
        font-size: 1.2rem;
    }

    .container {
        display: inline-block;
        margin: 5px;
        padding: 10px 10px;
        border: 0;
        line-height: 21px;
        cursor: pointer;
        color: #fff;
        background: #42b754;
        border-radius: 5px;
        font-size: 21px;
        outline: 0;
        width: 100px

        -webkit-touch-callout: none;
        -webkit-user-select: none;
        -khtml-user-select: none;
        -moz-user-select: none;
        -ms-user-select: none;

        user-select: none;
    }

    .container input:checked ~ .checkbox {
        background-color: #f32121;
    }

    button:hover {
        background: #ff494d
    }

    button:active {
        background: #f21c21
    }

    #servoTable {
        width: 100%;
        border-collapse: collapse;
        margin: 20px auto;
        outline: 2px solid #000000;
        font-family: Arial, sans-serif;
    }

    #servoTable th {
        background-color: #000000;
        color: #ffffff;
        padding: 20px;
        text-align: center;
        border-bottom: 1px solid #ddd;
    }

    #servoTable td {
        padding: 12px;
        border-bottom: 1px solid #ddd;
    }

    #servoTable tr:nth-child(even) {
        background-color: #bababa; /* Light background for even rows */
    }
    </style>
</head>

<body>
    <h2>WiFi Servo Tester</h2>
    <label align="center"><button class="button" id="rescanButton" onclick="rescan();">Rescan</button></label>
    <label><button class="button" id="stopButton" onclick="sendStop();">Stop</button></label>
    <table id="servoTable">
        <thead>
            <tr>
                <th>Id</th>
                <th>Enable?</th>
                <th>Target mode</th>
                <th>Target value</th>
                <th style="width:50%">State</th>
                <th>Set center</th>
                <th>Update id</th>
            </tr>
        </thead>
        <tbody>
        </tbody>
    </table>

    <script>
        function toggleCheckbox(inputT, inputI, inputA, inputB) {
            var xhr = new XMLHttpRequest();
            xhr.open("GET", "cmd?inputT="+inputT+"&inputI="+inputI+"&inputA="+inputA+"&inputB="+inputB, true);
            xhr.send();
        }


        setInterval(getData, 100);

        function getData() {
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function() {
                if (this.readyState == 4 && this.status == 200) {
                  responses = this.responseText.split(";").filter(Boolean);
                  for (response of responses)
                  {
                      subcommands = response.split("@").filter(Boolean);
                      command = subcommands.shift();

                      if (command == 'n')
                      {
                        tblBody = document.querySelector('#servoTable tbody');
                        const rowCount = tblBody.rows.length; // Get the number of rows

                        // Loop through rows in reverse and delete each one
                        for (let i = rowCount - 1; i >= 0; i--) {
                            tblBody.deleteRow(i);
                        }

                        for (let id of subcommands)
                        {
                            const row = document.createElement("tr");

                            cell = document.createElement("td");
                            const cellText = document.createTextNode(id);
                            cell.appendChild(cellText);
                            row.appendChild(cell);

                            let x = document.createElement("INPUT");
                            x.setAttribute("type", "checkbox");
                            x.addEventListener("click", function(){setEnable(x, id);}, true);
                            cell = document.createElement("td");
                            cell.appendChild(x);
                            row.appendChild(cell);

                            let list = document.createElement("select");
                            list.appendChild(new Option("Position", "0"));
                            list.appendChild(new Option("Velocity", "1"));
                            list.appendChild(new Option("Step", "3"));
                            cell = document.createElement("td");
                            cell.appendChild(list);
                            row.appendChild(cell);


                            let entry = document.createElement("input");
                            entry.type = "number";
                            entry.value = 0;
                            entry.addEventListener("change", function(){setTarget(entry, list, id);}, true);
                            cell = document.createElement("td");
                            cell.appendChild(entry);
                            row.appendChild(cell);

                            let l = document.createElement("LABEL");
                            l.id = "label" + id;
                            cell = document.createElement("td");
                            cell.appendChild(l);
                            row.appendChild(cell);

                            let b = document.createElement("button");
                            b.textContent = "Center";
                            b.id = "center" + id;
                            b.addEventListener("click", function(){setCenter(b, id);}, true);
                            cell = document.createElement("td");
                            cell.appendChild(b);
                            row.appendChild(cell);

                            let b2 = document.createElement("button");
                            b2.textContent = "Change id";
                            b2.id = "newId" + id;
                            b2.addEventListener("click", function(){setId(b, id);}, true);
                            cell = document.createElement("td");
                            cell.appendChild(b2);
                            row.appendChild(cell);

                            tblBody.appendChild(row);
                        }
                      }
                      else
                      {
                        id = command;
                        l = document.querySelector('#label' + id);
                        l.innerHTML = subcommands[0];
                      }
                  }
                }
            };
            xhttp.open("GET", "update", true);
            xhttp.send();
        }

    function setEnable(x, id) {
        var xhr = new XMLHttpRequest();
        xhr.open("GET", "cmd?id="+id+"&command=enable&value="+ (x.checked ? 1 : 0), true);
        xhr.send();
    }

    function setCenter(x, id) {
        var xhr = new XMLHttpRequest();
        xhr.open("GET", "cmd?id="+id+"&command=setCenter&value=0", true);
        xhr.send();
    }

    function setId(x, startId) {
        newId = parseInt(prompt("Enter new id for servo " + startId, startId));
        if (!isNaN(newId))
        {
            var xhr = new XMLHttpRequest();
            xhr.open("GET", "cmd?id="+startId+"&command=updateId&value="+startId+ "&value2=" + newId, true);
            xhr.send();
        }
    }

    function setTarget(x, mode, id) {
        var xhr = new XMLHttpRequest();
        xhr.open("GET", "cmd?id="+id+"&command=setTarget&value="+ x.value + "&value2=" + mode.value, true);
        xhr.send();
    }

    function rescan(){
        var xhr = new XMLHttpRequest();
        xhr.open("GET", "cmd?id=0&command=rescan&value=1", true);
        xhr.send();
    }

    function sendStop(){
        document.querySelectorAll('input[type=checkbox]').forEach(el => el.checked = false);
        var xhr = new XMLHttpRequest();
        xhr.open("GET", "cmd?id=0&command=stop&value=1", true);
        xhr.send();
    }

    </script>

</body>
</html>
)rawliteral";