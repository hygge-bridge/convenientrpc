aux_source_directory(. SRC_LIST)
# 由于muduo默认生成静态库，为了方便遍历，这里也生成静态库
add_library(convenientrpc ${SRC_LIST})
target_link_libraries(convenientrpc muduo_net muduo_base pthread)