THRDS_LIST=(2 4 8 16 32)

for nthd in ${!THRDS_LIST[*]}; do
    for index in {1..30}; do
        echo "Corriendo it n $index, con ${THRDS_LIST[$nthd]}"
        OMP_NUM_THREADS=${THRDS_LIST[$nthd]} ./main >> out_${THRDS_LIST[$nthd]}_thrds
        echo "*************************" >> out_${THRDS_LIST[$nthd]}_thrds
    done
done
