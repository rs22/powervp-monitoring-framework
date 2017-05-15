$.response.contentType = "text/html";
 
var conn = $.db.getConnection();
var pstmt = conn.prepareStatement(
`SELECT LPAR, (CASE WHEN test.REMOTE_L4_CPI < 1.0 THEN 'green' ELSE 'red' END) AS color, test.LSU_CPI, test.L2_CPI, test.REMOTE_L4_CPI, test.DISTANT_L4_CPI
FROM (
SELECT *,
        ROW_NUMBER() OVER (PARTITION BY LPAR ORDER BY REMOTE_L4_CPI ASC) AS RowAsc,
        COUNT(*) over (PARTITION BY LPAR) as RowCount
FROM (
    SELECT *, 
        ROW_NUMBER() OVER (Partition BY LPAR ORDER BY "TIMESTAMP" DESC) AS rn
    FROM LPAR_SAMPLES
    ) rs
WHERE rs.rn <= 5
) test
WHERE test.RowAsc = FLOOR((test.RowCount + 1) / 2) AND test.LPAR = 4
`);
var rs = pstmt.executeQuery();

var lpars = [];

if (!rs.next()) {
    $.response.setBody( "Failed to retrieve data" );
    $.response.status =  $.net.http.INTERNAL_SERVER_ERROR;
} else {
    do {
        lpars.push({ id: rs.getInteger(1), color: rs.getString(2), lsu: rs.getFloat(3), l2: rs.getFloat(4), remoteL4: rs.getFloat(5), distantL4: rs.getFloat(6)});
    } while(rs.next())
}
rs.close();
pstmt.close();
conn.close();

var lpar_names = {
    2: "ubp8le-01 (Ubuntu, shared)",
    4: "aix01 (AIX, shared)",
    6: "ubp8le-02 (Ubuntu, dedicated)"
};

$.response.setBody(`
<!DOCTYPE html>
<html>
<head>
<link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css" integrity="sha384-BVYiiSIFeK1dGmJRAkycuHAHRg32OmUcww7on3RYdg4Va+PmSTsz/K68vbdEjh4u" crossorigin="anonymous">
<link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap-theme.min.css" integrity="sha384-rHyoN1iRsVXV4nD0JutlnGaslCJuC7uwjduW9SVrLvRYooPp2bWYgmgJQIXwl/Sp" crossorigin="anonymous">
</head>
<body>
<div class="container">

    <h2>Monitor LPARs</h2>
    
    <p>The displayed values are medians of the last five measurements (1 measurement per second)</p>

    <table class="table table-striped">
        <thead>
            <tr><td>LPAR</td><td>Status</td><td>Avg LSU CPI</td><td>Local L2 CPI</td><td>Remote L4 CPI</td><td>Distant L4 CPI</td></tr>
        </thead>
        <tbody>
            ${lpars.map(lpar => `<tr><td>${lpar_names[lpar.id]}</td><td class='${lpar.color === "red" ? "danger" : ""}'>${lpar.color === "red" ? "Warning" : "OK"}</td><td>${lpar.lsu.toFixed(4)} (100%)</td><td>${lpar.l2.toFixed(4)} (${(lpar.l2 / lpar.lsu * 100).toFixed(0)}%)</td><td>${lpar.remoteL4.toFixed(4)} (${(lpar.remoteL4 / lpar.lsu * 100).toFixed(0)}%)</td><td>${lpar.distantL4.toFixed(4)} (${(lpar.distantL4 / lpar.lsu * 100).toFixed(0)}%)</td></tr>`).join('\n')}
        </tbody>
    </table>
</div>
<script src="https://code.jquery.com/jquery-3.1.1.min.js" integrity="sha256-hVVnYaiADRTO2PzUGmuLJr8BLUSjGIZsDYGmIJLv2b8=" crossorigin="anonymous"></script>
<script src="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js" integrity="sha384-Tc5IQib027qvyjSMfHjOMaLkfuWVxZxUPnCJA7l2mCWNIpG9mGCD8wGNIcPD7Txa" crossorigin="anonymous"></script>
<script type="text/javascript">
    setTimeout(function() {
        location.reload();
    }, 1000);
</script>
</body>
</html>
`);