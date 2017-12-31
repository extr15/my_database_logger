clear;

conn = sqlite('../build/my_debug.db', 'readonly');
reprojErrTableNumsCell = fetch(conn, 'SELECT count(*) FROM sqlite_master WHERE type = ''table'' AND name LIKE ''table_reproj_err_%'' ');
reprojErrTableNums = reprojErrTableNumsCell{1};

curReprojErrTableName=['table_reproj_err_', num2str(reprojErrTableNums - 1)];
reprojErrCell = fetch(conn, ['SELECT * FROM ', curReprojErrTableName]);
rowReprojErrCell = length(reprojErrCell);
reprojErr = [reprojErrCell{:,2}, reprojErrCell{:,3}];
reprojErr = reshape(reprojErr, rowReprojErrCell, 2);

close(conn);

figure;
title('reprojErr');
hold on;
grid on;
plot(reprojErr(:, 1));
plot(reprojErr(:, 2));

